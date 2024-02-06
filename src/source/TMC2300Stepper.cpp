#include "TMCStepper.h"
#include "TMC_MACROS.h"
#include "SERIAL_SWITCH.h"

// Protected
// addr needed for TMC2209
TMC2300Stepper::TMC2300Stepper(Stream * SerialPort, float RS, uint8_t addr) :
	TMCStepper(RS),
	slave_address(addr)
	{
		HWSerial = SerialPort;
		defaults();
	}

TMC2300Stepper::TMC2300Stepper(Stream * SerialPort, float RS, uint8_t addr, uint16_t mul_pin1, uint16_t mul_pin2) :
	TMC2300Stepper(SerialPort, RS)
	{
		SSwitch *SMulObj = new SSwitch(mul_pin1, mul_pin2, addr);
		sswitch = SMulObj;
	}

#if SW_CAPABLE_PLATFORM
	// Protected
	// addr needed for TMC2209
	TMC2300Stepper::TMC2300Stepper(uint16_t SW_RX_pin, uint16_t SW_TX_pin, float RS, uint8_t addr) :
		TMCStepper(RS),
		RXTX_pin(SW_RX_pin == SW_TX_pin ? SW_RX_pin : 0),
		slave_address(addr)
		{
			SoftwareSerial *SWSerialObj = new SoftwareSerial(SW_RX_pin, SW_TX_pin);
			SWSerial = SWSerialObj;
			defaults();
		}

	void TMC2300Stepper::beginSerial(uint32_t baudrate) {
		if (SWSerial != nullptr)
		{
			SWSerial->begin(baudrate);
			SWSerial->end();
		}
		#if defined(ARDUINO_ARCH_AVR)
			if (RXTX_pin > 0) {
				digitalWrite(RXTX_pin, HIGH);
				pinMode(RXTX_pin, OUTPUT);
			}
		#endif
	}
#endif

void TMC2300Stepper::begin() {
	#if SW_CAPABLE_PLATFORM
		beginSerial(115200);
	#endif

}

void TMC2300Stepper::defaults() {
	IHOLD_IRUN_register.iholddelay = 1; // OTP
	CHOPCONF_register.sr = 0x13008001;
	PWMCONF_register.sr = 0xC40D1024;
  //MSLUT0_register.sr = ???;
  //MSLUT1_register.sr = ???;
  //MSLUT2_register.sr = ???;
  //MSLUT3_register.sr = ???;
  //MSLUT4_register.sr = ???;
  //MSLUT5_register.sr = ???;
  //MSLUT6_register.sr = ???;
  //MSLUT7_register.sr = ???;
  //MSLUTSTART_register.start_sin90 = 247;
}

void TMC2300Stepper::push() {
	GCONF(GCONF_register.sr);
	IHOLD_IRUN(IHOLD_IRUN_register.sr);
	SLAVECONF(SLAVECONF_register.sr);
	TPOWERDOWN(TPOWERDOWN_register.sr);
	TPWMTHRS(TPWMTHRS_register.sr);
	VACTUAL(VACTUAL_register.sr);
	CHOPCONF(CHOPCONF_register.sr);
	PWMCONF(PWMCONF_register.sr);
}

bool TMC2300Stepper::isEnabled() { return !enn() && toff(); }

uint8_t TMC2300Stepper::calcCRC(uint8_t datagram[], uint8_t len) {
	uint8_t crc = 0;
	for (uint8_t i = 0; i < len; i++) {
		uint8_t currentByte = datagram[i];
		for (uint8_t j = 0; j < 8; j++) {
			if ((crc >> 7) ^ (currentByte & 0x01)) {
				crc = (crc << 1) ^ 0x07;
			} else {
				crc = (crc << 1);
			}
			crc &= 0xff;
			currentByte = currentByte >> 1;
		}
	}
	return crc;
}

__attribute__((weak))
int TMC2300Stepper::available() {
	int out = 0;
	#if SW_CAPABLE_PLATFORM
		if (SWSerial != nullptr) {
			out = SWSerial->available();
		} else
	#endif
		if (HWSerial != nullptr) {
			out = HWSerial->available();
		}

	return out;
}

__attribute__((weak))
void TMC2300Stepper::preWriteCommunication() {
	if (HWSerial != nullptr) {
		if (sswitch != nullptr)
			sswitch->active();
	}
}

__attribute__((weak))
void TMC2300Stepper::preReadCommunication() {
	#if SW_CAPABLE_PLATFORM
		if (SWSerial != nullptr) {
			SWSerial->listen();
		} else
	#endif
		if (HWSerial != nullptr) {
			if (sswitch != nullptr)
				sswitch->active();
		}
}

__attribute__((weak))
int16_t TMC2300Stepper::serial_read() {
	int16_t out = 0;
	#if SW_CAPABLE_PLATFORM
		if (SWSerial != nullptr) {
			out = SWSerial->read();
		} else
	#endif
		if (HWSerial != nullptr) {
			out = HWSerial->read();
		}

	return out;
}

__attribute__((weak))
uint8_t TMC2300Stepper::serial_write(const uint8_t data) {
	int out = 0;;
	#if SW_CAPABLE_PLATFORM
		if (SWSerial != nullptr) {
			return SWSerial->write(data);
		} else
	#endif
		if (HWSerial != nullptr) {
			return HWSerial->write(data);
		}

	return out;
}

__attribute__((weak))
void TMC2300Stepper::postWriteCommunication() {}

__attribute__((weak))
void TMC2300Stepper::postReadCommunication() {
	#if SW_CAPABLE_PLATFORM
		if (SWSerial != nullptr) {
			SWSerial->end();
		}
	#endif
}

void TMC2300Stepper::write(uint8_t addr, uint32_t regVal) {
	uint8_t len = 7;
	addr |= TMC_WRITE;
	uint8_t datagram[] = {TMC2300_SYNC, slave_address, addr, (uint8_t)(regVal>>24), (uint8_t)(regVal>>16), (uint8_t)(regVal>>8), (uint8_t)(regVal>>0), 0x00};

	datagram[len] = calcCRC(datagram, len);

	preWriteCommunication();

	for(uint8_t i=0; i<=len; i++) {
		bytesWritten += serial_write(datagram[i]);
	}
	postWriteCommunication();

	delay(replyDelay);
}

uint64_t TMC2300Stepper::_sendDatagram(uint8_t datagram[], const uint8_t len, uint16_t timeout) {
	while (available() > 0) serial_read(); // Flush

	#if defined(ARDUINO_ARCH_AVR)
		if (RXTX_pin > 0) {
			digitalWrite(RXTX_pin, HIGH);
			pinMode(RXTX_pin, OUTPUT);
		}
	#endif

	for(int i=0; i<=len; i++) serial_write(datagram[i]);

	#if defined(ARDUINO_ARCH_AVR)
		if (RXTX_pin > 0) {
			pinMode(RXTX_pin, INPUT_PULLUP);
		}
	#endif

	delay(this->replyDelay);

	// scan for the rx frame and read it
	uint32_t ms = millis();
	uint32_t sync_target = (static_cast<uint32_t>(datagram[0])<<16) | 0xFF00 | datagram[2];
	uint32_t sync = 0;

	do {
		uint32_t ms2 = millis();
		if (ms2 != ms) {
			// 1ms tick
			ms = ms2;
			timeout--;
		}
		if (!timeout) return 0;

		int16_t res = serial_read();
		if (res < 0) continue;

		sync <<= 8;
		sync |= res & 0xFF;
		sync &= 0xFFFFFF;

	} while (sync != sync_target);

	uint64_t out = sync;
	ms = millis();
	timeout = this->abort_window;

	for(uint8_t i=0; i<5;) {
		uint32_t ms2 = millis();
		if (ms2 != ms) {
			// 1ms tick
			ms = ms2;
			timeout--;
		}
		if (!timeout) return 0;

		int16_t res = serial_read();
		if (res < 0) continue;

		out <<= 8;
		out |= res & 0xFF;

		i++;
	}

	#if defined(ARDUINO_ARCH_AVR)
		if (RXTX_pin > 0) {
			digitalWrite(RXTX_pin, HIGH);
			pinMode(RXTX_pin, OUTPUT);
		}
	#endif

	while (available() > 0) serial_read(); // Flush

	return out;
}

uint32_t TMC2300Stepper::read(uint8_t addr) {
	constexpr uint8_t len = 3;
	addr |= TMC_READ;
	uint8_t datagram[] = {TMC2300_SYNC, slave_address, addr, 0x00};
	datagram[len] = calcCRC(datagram, len);
	uint64_t out = 0x00000000UL;

	for (uint8_t i = 0; i < max_retries; i++) {
		preReadCommunication();
		out = _sendDatagram(datagram, len, abort_window);
		postReadCommunication();

		delay(replyDelay);

		CRCerror = false;
		uint8_t out_datagram[] = {
			static_cast<uint8_t>(out>>56),
			static_cast<uint8_t>(out>>48),
			static_cast<uint8_t>(out>>40),
			static_cast<uint8_t>(out>>32),
			static_cast<uint8_t>(out>>24),
			static_cast<uint8_t>(out>>16),
			static_cast<uint8_t>(out>> 8),
			static_cast<uint8_t>(out>> 0)
		};
		uint8_t crc = calcCRC(out_datagram, 7);
		if ((crc != static_cast<uint8_t>(out)) || crc == 0 ) {
			CRCerror = true;
			out = 0;
		} else {
			break;
		}
	}

	return out>>8;
}

uint8_t TMC2300Stepper::IFCNT() {
	return read(IFCNT_t::address);
}

void TMC2300Stepper::SLAVECONF(uint16_t input) {
	SLAVECONF_register.sr = input&0xF00;
	write(SLAVECONF_register.address, SLAVECONF_register.sr);
}
uint16_t TMC2300Stepper::SLAVECONF() {
	return SLAVECONF_register.sr;
}
void TMC2300Stepper::senddelay(uint8_t B) 	{ SLAVECONF_register.senddelay = B; write(SLAVECONF_register.address, SLAVECONF_register.sr); }
uint8_t TMC2300Stepper::senddelay() 		{ return SLAVECONF_register.senddelay; }

uint32_t TMC2300Stepper::IOIN() {
	return read(TMC2300_n::IOIN_t::address);
}
bool TMC2300Stepper::enn()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.enn;		}
bool TMC2300Stepper::ms1()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms1;		}
bool TMC2300Stepper::ms2()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms2;		}
bool TMC2300Stepper::diag()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.diag;		}
bool TMC2300Stepper::pdn_uart()		{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.pdn_uart;	}
bool TMC2300Stepper::step()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.step;		}
bool TMC2300Stepper::sel_a()		{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.sel_a;	}
bool TMC2300Stepper::dir()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.dir;		}
uint8_t TMC2300Stepper::version() 	{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.version;	}

uint32_t TMC2224Stepper::IOIN() {
	return read(TMC2224_n::IOIN_t::address);
}
bool TMC2224Stepper::enn()			{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.enn;		}
bool TMC2224Stepper::ms1()			{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms1;		}
bool TMC2224Stepper::ms2()			{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.ms2;		}
bool TMC2224Stepper::pdn_uart()		{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.pdn_uart;	}
bool TMC2224Stepper::spread()		{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.spread;	}
bool TMC2224Stepper::step()			{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.step;		}
bool TMC2224Stepper::sel_a()		{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.sel_a;	}
bool TMC2224Stepper::dir()			{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.dir;		}
uint8_t TMC2224Stepper::version() 	{ TMC2224_n::IOIN_t r{0}; r.sr = IOIN(); return r.version;	}

uint16_t TMC2300Stepper::FACTORY_CONF() {
	return read(FACTORY_CONF_register.address);
}
void TMC2300Stepper::FACTORY_CONF(uint16_t input) {
	FACTORY_CONF_register.sr = input;
	write(FACTORY_CONF_register.address, FACTORY_CONF_register.sr);
}
void TMC2300Stepper::fclktrim(uint8_t B){ FACTORY_CONF_register.fclktrim = B; write(FACTORY_CONF_register.address, FACTORY_CONF_register.sr); }
void TMC2300Stepper::ottrim(uint8_t B)	{ FACTORY_CONF_register.ottrim = B; write(FACTORY_CONF_register.address, FACTORY_CONF_register.sr); }
uint8_t TMC2300Stepper::fclktrim()		{ FACTORY_CONF_t r{0}; r.sr = FACTORY_CONF(); return r.fclktrim; }
uint8_t TMC2300Stepper::ottrim()		{ FACTORY_CONF_t r{0}; r.sr = FACTORY_CONF(); return r.ottrim; }

void TMC2300Stepper::VACTUAL(uint32_t input) {
	VACTUAL_register.sr = input;
	write(VACTUAL_register.address, VACTUAL_register.sr);
}
uint32_t TMC2300Stepper::VACTUAL() {
	return VACTUAL_register.sr;
}

uint32_t TMC2300Stepper::PWM_SCALE() {
	return read(TMC2300_n::PWM_SCALE_t::address);
}
uint8_t TMC2300Stepper::pwm_scale_sum() {
	TMC2300_n::PWM_SCALE_t r{0};
	r.sr = PWM_SCALE();
	return r.pwm_scale_sum;
}

int16_t TMC2300Stepper::pwm_scale_auto() {
	TMC2300_n::PWM_SCALE_t r{0};
	r.sr = PWM_SCALE();
	return r.pwm_scale_auto;
	// Not two's complement? 9nth bit determines sign
	/*
	uint32_t d = PWM_SCALE();
	int16_t response = (d>>PWM_SCALE_AUTO_bp)&0xFF;
	if (((d&PWM_SCALE_AUTO_bm) >> 24) & 0x1) return -response;
	else return response;
	*/
}

// R: PWM_AUTO
uint32_t TMC2300Stepper::PWM_AUTO() {
	return read(PWM_AUTO_t::address);
}
uint8_t TMC2300Stepper::pwm_ofs_auto()  { PWM_AUTO_t r{0}; r.sr = PWM_AUTO(); return r.pwm_ofs_auto; }
uint8_t TMC2300Stepper::pwm_grad_auto() { PWM_AUTO_t r{0}; r.sr = PWM_AUTO(); return r.pwm_grad_auto; }
