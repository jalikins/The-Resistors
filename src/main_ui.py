import os
from kivy.config import Config

# forcing UI to use full screen
Config.set('graphics', 'width', '800')
Config.set('graphics', 'height', '480')
# cover homebar
Config.set('graphics', 'fullscreen', 'auto') 
# disable cursor to make the ui feel more interactive
Config.set('graphics', 'show_cursor', '0')

# imports
import json
import serial
import time
import subprocess  
from kivy.app import App
from kivy.clock import Clock 
from kivy.core.window import Window
from kivy.uix.button import Button
from kivy.uix.gridlayout import GridLayout
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.properties import NumericProperty

# getting JSON path
SETTINGS_FILE = "bin_settings.json"

# setting theme colors
HIVE_BROWN_BG = (0.15, 0.10, 0.05, 1)   
HONEY_GOLD    = (1.0, 0.75, 0.0, 1)     
BEE_STRIPE    = (0.2, 0.1, 0.0, 1)      
WAX_PALE      = (1.0, 0.95, 0.7, 1)     
ROYAL_JELLY   = (0.9, 0.3, 0.1, 1)      
LEAF_GREEN    = (0.4, 0.7, 0.2, 1)      
DANGER_RED    = (0.8, 0.1, 0.1, 1)      

# bin button class
class BinButton(Button):
    bin_index = NumericProperty(0)
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.background_normal = ''
        self.background_color = HONEY_GOLD
        self.color = BEE_STRIPE  
        self.font_size = '18sp'  
        self.halign = 'center'

class NumpadPopup(Popup):
    def __init__(self, target_button, update_callback, **kwargs):
        super().__init__(**kwargs)
        self.title = f"Set Resistance for Bin {target_button.bin_index}"
        self.target_button = target_button
        self.update_callback = update_callback
        self.size_hint = (0.9, 0.9)
        self.auto_dismiss = False
        
        self.title_color = HONEY_GOLD 
        self.separator_color = HONEY_GOLD
        
        layout = BoxLayout(orientation='vertical', padding=10, spacing=10)
        self.display_label = Label(text="0", font_size='50sp', size_hint_y=0.25, color=WAX_PALE)
        layout.add_widget(self.display_label)
        
        grid = GridLayout(cols=3, spacing=10, size_hint_y=0.75)
        keys = ['1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', 'OK']
        
        for key in keys:
            btn = Button(text=key, font_size='30sp', background_normal='')
            if key == 'C':
                btn.background_color = ROYAL_JELLY
                btn.color = (1, 1, 1, 1) 
                btn.bind(on_release=self.clear_display)
            elif key == 'OK':
                btn.background_color = LEAF_GREEN
                btn.color = (1, 1, 1, 1) 
                btn.bind(on_release=self.confirm_value)
            else:
                btn.background_color = WAX_PALE
                btn.color = BEE_STRIPE
                btn.bind(on_release=self.on_num_press)
            grid.add_widget(btn)
        
        layout.add_widget(grid)
        self.content = layout

    def on_num_press(self, instance):
        if self.display_label.text == "0":
            self.display_label.text = instance.text
        else:
            self.display_label.text += instance.text

    def clear_display(self, instance):
        self.display_label.text = "0"

    def confirm_value(self, instance):
        new_val = self.display_label.text
        self.target_button.text = f"Bin {self.target_button.bin_index}\n{new_val} Ω"
        self.update_callback(self.target_button.bin_index, int(new_val))
        self.dismiss()

class SorterApp(App):
    current_data = {} 
    ser = None
    serial_buffer = "" 

    def build(self):
        Window.clearcolor = HIVE_BROWN_BG
        self.init_serial()
        self.load_settings()

        # Main Vertical Layout
        main_layout = BoxLayout(orientation='vertical', padding=10, spacing=10)

        # 1. THE GRID (Top Section)
        # We remove size_hint_y so it automatically takes all remaining space
        grid_layout = GridLayout(cols=4, spacing=10) 
        self.buttons = {} 

        for i in range(1, 14):
            btn = BinButton()
            btn.bin_index = i
            
            if str(i) in self.current_data:
                saved_val = self.current_data[str(i)]
                btn.text = f"Bin {i}\n{saved_val} Ω"
            else:
                self.current_data[str(i)] = 0
                btn.text = f"Bin {i}\n0 Ω"

            btn.bind(on_release=self.open_numpad)
            self.buttons[i] = btn
            grid_layout.add_widget(btn)
            
        main_layout.add_widget(grid_layout)

        # 2. CONTROL PANEL (Bottom Section)
        # FIX: We use size_hint_y=None and a fixed height (e.g., 80 pixels)
        # This guarantees the buttons are never squashed or pushed off screen.
        control_panel = BoxLayout(
            orientation='horizontal', 
            spacing=20, 
            size_hint_y=None, 
            height=80
        )
        
        start_btn = Button(text="START SORTER", font_size='20sp', bold=True, background_normal='')
        start_btn.background_color = LEAF_GREEN
        start_btn.bind(on_release=self.send_start_signal)
        
        stop_btn = Button(text="STOP SORTER", font_size='20sp', bold=True, background_normal='')
        stop_btn.background_color = DANGER_RED
        stop_btn.bind(on_release=self.send_stop_signal)

        control_panel.add_widget(start_btn)
        control_panel.add_widget(stop_btn)
        
        main_layout.add_widget(control_panel)

        # Listens 10 times a second for serial messages
        Clock.schedule_interval(self.check_serial_messages, 0.1) 
        
        return main_layout

    # serial communication
    def init_serial(self):
        """Attempts to connect to the Arduino/PCB via USB."""
        try:
            # trying most common usb port types
            port_to_use = '/dev/ttyUSB0' 
            if not os.path.exists(port_to_use):
                port_to_use = '/dev/ttyACM0'
            
            # timeout=0 is important for non-blocking read
            self.ser = serial.Serial(port_to_use, 9600, timeout=0)
            print(f"Connected to {port_to_use}")
        except Exception as e:
            print(f"SERIAL ERROR: Could not connect to hardware. Running in dummy mode. ({e})")
            self.ser = None

    def send_serial_data(self, string_data):
        """Sends data over serial with a newline character."""
        msg = f"{string_data}\n"
        
        if self.ser:
            try:
                self.ser.write(msg.encode('utf-8'))
                print(f"Sent Serial: {msg.strip()}")
            except Exception as e:
                print(f"Failed to send: {e}")
        else:
            # dummy mode for testing UI without hardware
            print(f"[MOCK SERIAL] Would send: {msg.strip()}")

    def send_start_signal(self, instance):
        # sends start signal to PCB
        self.send_serial_data("go")

    def send_stop_signal(self, instance):
        # sends stop signal to PCB
        self.send_serial_data("stop")

    def broadcast_all_values(self):
        """
        Compiles values for Bin 1 to 13 into a list and sends it.
        Index 0 = Bin 1, Index 12 = Bin 13.
        """
        values_list = []
        for i in range(1, 14):
            # Fetch value, default to 0 if missing (safety)
            val = self.current_data.get(str(i), 0)
            values_list.append(val)
        
        # Convert list to JSON string (e.g., "[100, 220, 330...]")
        json_payload = json.dumps(values_list)
        self.send_serial_data(json_payload)

    # ui interation 
    def open_numpad(self, instance):
        popup = NumpadPopup(target_button=instance, update_callback=self.update_bin_value)
        popup.open()

    def update_bin_value(self, bin_id, resistance_value):
        # 1. Update internal data
        self.current_data[str(bin_id)] = resistance_value
        
        # 2. Save to file
        self.save_settings()
        
        # 3. Send NEW list to hardware immediately
        self.broadcast_all_values()

    def save_settings(self):
        try:
            with open(SETTINGS_FILE, 'w') as f:
                json.dump(self.current_data, f)
            print("Settings saved.")
        except Exception as e:
            print(f"Error saving: {e}")

    def load_settings(self):
        if os.path.exists(SETTINGS_FILE):
            try:
                with open(SETTINGS_FILE, 'r') as f:
                    data = json.load(f)
                    self.current_data = data
            except Exception as e:
                print(f"Error loading: {e}")
                self.current_data = {}

    # --- NEW SERIAL LISTENER ---
    def check_serial_messages(self, dt):
        """Called every 0.1s to read incoming data from Arduino."""
        if self.ser and self.ser.in_waiting > 0:
            try:
                # Read data and decode
                incoming = self.ser.read(self.ser.in_waiting).decode('utf-8')
                self.serial_buffer += incoming
                
                # Process complete lines
                while '\n' in self.serial_buffer:
                    line, self.serial_buffer = self.serial_buffer.split('\n', 1)
                    line = line.strip() # Remove whitespace
                    if line:
                        self.process_command(line)
                    
            except Exception as e:
                print(f"Read Error: {e}")

    def process_command(self, command):
        # turns on screen
        if command == "ready":
            # 1. Reply to the Nano so it stops spamming
            # Only send if we haven't recently (basic spam protection)
            print("Received 'ready', waking screen...")
            self.send_serial_data("received")
            
            # 2. Force Screen ON
            subprocess.run("export DISPLAY=:0 && xset dpms force on", shell=True)

        # LOGIC 2: Turn Screen OFF ("deactivated")
        elif command == "deactivated":
            print("Received 'deactivated', blanking screen...")
            # Force Screen OFF
            subprocess.run("export DISPLAY=:0 && xset dpms force off", shell=True)

if __name__ == '__main__':
    SorterApp().run()