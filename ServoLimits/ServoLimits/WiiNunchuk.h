/*
 * WiiNunchuk
 *
 * This code owes thanks to the code by these authors:
 * Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code is taken from Windmeadow Labs
 * http://www.windmeadow.com/node/42
 *
 * and the reference document on the wii linux site:
 * http://www.wiili.org/index.php/Wiimote/Extension_Controllers/Classic_Controller
 */

#include "Wire.h"


#ifndef WiiNunchuk_h
#define WiiNunchuk_h

typedef uint8_t byte;
typedef uint8_t boolean;

class WiiNunchuk {
    private:
        byte cnt;
        uint8_t data[6];		// array to store wiichuck output
        byte averageCounter;

    public:

        void begin()
        {
            Wire.begin();
            cnt = 0;
            averageCounter = 0;
            Wire.beginTransmission (0x52);	// transmit to device 0x52
            Wire.send (0x40);		// sends memory address
            Wire.send (0x00);		// sends memory address
            Wire.endTransmission ();	// stop transmitting

            update();

        }


        void update() {

            Wire.requestFrom (0x52, 6);	// request data from nunchuck
            while (Wire.available ()) {
                // receive byte as an integer
                data[cnt] = _nunchuk_decode_byte (Wire.receive()); //_nunchuk_decode_byte ()
                cnt++;
            }
            if (cnt > 5) {
                _send_zero(); // send the request for next bytes
                cnt = 0;
            }
        }

		/*
			The joystick x value
		*/
		byte x() {
			return data[0];
		}

		/*
			The joystick y value
		*/
		byte y() {
			return data[1];
		}

		/*
			The roll value (without the low 2 bits)
		*/
		byte xacc() {
			return data[2];
		}
		/*
			The roll value (without the low 2 bits)
		*/
		byte yacc() {
			return data[3];
		}

		/*
			The roll value (without the low 2 bits)
		*/
		byte zacc() {
			return data[4];
		}

		/*
			The z button
		*/
		byte z() {
			return (~data[5]) & 0x01;
		}

		/*
			The c button
		*/
		byte c() {
			return (~data[5]>>1) & 0x01;
		}

    private:

        byte _nunchuk_decode_byte (byte x)
        {
            x = (x ^ 0x17) + 0x17;
            return x;
        }

        void _send_zero()
        {
            Wire.beginTransmission (0x52);	// transmit to device 0x52
            Wire.send (0x00);		// sends one byte
            Wire.endTransmission ();	// stop transmitting
        }

};



#endif
