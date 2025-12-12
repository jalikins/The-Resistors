import json
import os
from kivy.app import App
from kivy.core.window import Window
from kivy.uix.button import Button
from kivy.uix.gridlayout import GridLayout
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.properties import NumericProperty

SETTINGS_FILE = "bin_settings.json"

# menu colors
HIVE_BROWN_BG = (0.15, 0.10, 0.05, 1)   
HONEY_GOLD    = (1.0, 0.75, 0.0, 1)     
BEE_STRIPE    = (0.2, 0.1, 0.0, 1)      
WAX_PALE      = (1.0, 0.95, 0.7, 1)     
ROYAL_JELLY   = (0.9, 0.3, 0.1, 1)      
LEAF_GREEN    = (0.4, 0.7, 0.2, 1)      

class BinButton(Button):
    bin_index = NumericProperty(0)
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # remove default texture 
        self.background_normal = ''
        self.background_color = HONEY_GOLD
        self.color = BEE_STRIPE  
        self.font_size = '22sp'
        self.halign = 'center'

class NumpadPopup(Popup):
    def __init__(self, target_button, update_callback, **kwargs):
        super().__init__(**kwargs)
        self.title = f"Set Resistance for Bin {target_button.bin_index}"
        self.target_button = target_button
        self.update_callback = update_callback
        self.size_hint = (0.9, 0.9)
        self.auto_dismiss = False
        
        # style popup background
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
                # number keys
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

    def build(self):
        # set main window background color
        Window.clearcolor = HIVE_BROWN_BG
        
        self.load_settings()

        # configuring main layout
        main_layout = BoxLayout(orientation='vertical', padding=10, spacing=10)

        # grid of buttons for bins 
        grid_layout = GridLayout(cols=4, spacing=10, size_hint_y=1)
        self.buttons = {} 

        for i in range(1, 13):
            # using custom themed button class
            btn = BinButton()
            btn.bin_index = i
            
            if str(i) in self.current_data:
                saved_val = self.current_data[str(i)]
                btn.text = f"Bin {i}\n{saved_val} Ω"
            else:
                btn.text = f"Bin {i}\n--"

            btn.bind(on_release=self.open_numpad)
            self.buttons[i] = btn
            grid_layout.add_widget(btn)
            
        main_layout.add_widget(grid_layout)
        return main_layout

    def open_numpad(self, instance):
        popup = NumpadPopup(target_button=instance, update_callback=self.update_bin_value)
        popup.open()

    def update_bin_value(self, bin_id, resistance_value):
        self.current_data[str(bin_id)] = resistance_value
        self.save_settings()

    # saving settings to JSON
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
                    if "bins" in data:
                        self.current_data = data["bins"]
                    else:
                        self.current_data = data
            except Exception as e:
                print(f"Error loading: {e}")
                self.current_data = {}

if __name__ == '__main__':
    SorterApp().run()