#include "TMCStepper.h"
#include "TMC_MACROS.h"
#include "SERIAL_SWITCH.h"



void TMC2300Stepper::begin() {
	#if SW_CAPABLE_PLATFORM
		beginSerial(115200);
	#endif

}

void TMC2300Stepper::defaults() {
	IHOLD_IRUN_register.iholddelay = 1; // OTP
	CHOPCONF_register.sr = 0x13008001;
	PWMCONF_register.sr = 0xC40D1024;
  //COOLCONF_register.sr = ???; 
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
    //COOLCONF(COOLCONF_registered.sr);
}

bool TMC2300Stepper::isEnabled() { return !enn() && toff(); }


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

bool TMC2300Stepper::diag()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.diag;		}
bool TMC2300Stepper::pdn_uart()		{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.pdn_uart;	}
bool TMC2300Stepper::step()			{ TMC2300_n::IOIN_t r{0}; r.sr = IOIN(); return r.step;		}
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
