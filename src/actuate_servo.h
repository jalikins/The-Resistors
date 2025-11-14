#include <Servo.h>
#include <math.h>
#include <MeasureResistance.h>

int actuateServo(float beltPos) {
    for (i=o; i<=sizeof(binOrder[0]); i+=1) {
        if round(beltPos - i) == 0 {
            servo1.write(servoAngle);
        }
    }
    for (i=o; i<=sizeof(binOrder[1]); i+=1) {
        if round(beltPos - i) == 0 {
            servo2.write(servoAngle);
        }
    }
    for (i=o; i<=sizeof(binOrder[2]); i+=1) {
        if round(beltPos - i) == 0 {
            servo3.write(servoAngle);
        }
    }
    for (i=o; i<=sizeof(binOrder[3]); i+=1) {
        if round(beltPos - i) == 0 {
            servo4.write(servoAngle);
        }
    }
}