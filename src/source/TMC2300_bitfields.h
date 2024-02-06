#pragma once
#pragma pack(push, 1)

namespace TMC2300_n {
struct GCONF_t {
  constexpr static uint8_t address = 0x00;
  union {
    uint32_t sr : 10;
    struct {
      bool  : 1,
            extcap : 1,
            : 1,
            shaft : 1,
            diag_step: 1,
            multistep_filt: 1,
            test_mode: 1;
    };
  };
};

struct GSTAT_t {
  constexpr static uint8_t address = 0x01;
  union {
    uint8_t sr : 3;
    struct {
      bool  reset : 1,
            drv_err : 1,
            u3v5 : 1;
    };
  };
};
struct SLAVECONF_t {
  constexpr static uint8_t address = 0x03;
  union {
    uint16_t sr : 12;
    struct {
      uint8_t : 8,
                senddelay: 4;
    };
  };
};


struct IOIN_t {
  constexpr static uint8_t address = 0x04;
  union {
    uint32_t sr : 32;
    struct {
      bool  en : 1,
            nstdby : 1,
            ad0 : 1,
            ad1 : 1,
            diag : 1,
            stepper : 1,
            pdn_uart : 1,
            step: 1,
            dir: 1,
            comp_a1a2: 1,
            comp_b1b2: 1;
      uint16_t : 12;
      uint8_t version : 8;
    };
  };
};

struct IHOLD_IRUN_t {
  constexpr static uint8_t address = 0x10;
  union {
    uint32_t sr : 20;
    struct {
      uint8_t ihold : 5,
                    : 3,
              irun : 5,
                   : 3,
              iholddelay : 4;
    };
  };
};

struct TPOWERDOWN_t {
  constexpr static uint8_t address = 0x11;
  uint8_t sr : 8;
};

struct TSTEP_t {
  constexpr static uint8_t address = 0x12;
  uint32_t sr : 20;
};

struct VACTUAL_t {
  constexpr static uint8_t address = 0x22;
  uint32_t sr : 24;
};

struct TCOOLTHRS_t {
  constexpr static uint8_t address = 0x14;
  uint32_t sr : 10;
};

struct SGTHRS_t {
  constexpr static uint8_t address = 0x15;
  uint32_t sr : 8;
};

struct SG_VALUE_t {
  constexpr static uint8_t address = 0x42;
  uint16_t sr : 10;
};

struct COOLCONF_t {
  constexpr static uint8_t address = 0x42;
  union {
    uint32_t sr : 16;
    struct {
      int8_t semin : 4;
      bool : 1;
      int8_t seup : 2;
      bool : 1;
      int8_t semax : 4;
      bool : 1;
      int8_t sedn : 2;
      bool seimin : 1,
            : 1;
    };
  };
};

struct MSCNT_t {
  constexpr static uint8_t address = 0x6a;
  uint16_t sr : 10;
};

//struct COOLCONF_t {
//  constexpr static uint8_t address = 0x42;
//  union {
//    uint32_t sr : 16;
//    struct {
//      uint8_t semin : 4,
//                    : 1,
//              seup : 2,
//                    : 1,
//              semax : 4,
//                    : 1,
//              sedn : 2;
//      bool    seimin : 1;
//      int8_t  sgt : 7,
//                  : 1;
//      bool    sfilt : 1;
//    };
//  };
//};

struct CHOPCONF_t {
  constexpr static uint8_t address = 0x6C;
  union {
    uint32_t sr : 32;
    struct {
      uint16_t : 15;
      uint8_t tbl : 2,
              : 7,
              mres : 4;
      bool    intpol : 1,
              dedge : 1,
              diss2g : 1,
              diss2vs : 1;
    };
  };
};

struct PWMCONF_t {
  constexpr static uint8_t address = 0x70;
  union {
    uint32_t sr : 32;
    struct {
      uint8_t pwm_ofs : 8,
              pwm_grad : 8,
              pwm_freq : 2;
      bool pwm_autoscale : 1,
           pwm_autograd : 1;
      uint8_t freewheel : 2,
              : 2,
              pwm_reg : 4,
              pwm_lim : 4;

    };
  };
};


  struct DRV_STATUS_t {
    constexpr static uint8_t address = 0x6F;
    union {
      uint32_t sr;
      struct {
        bool  otpw : 1,
              ot : 1,
              s2ga : 1,
              s2gb : 1,
              s2vsa : 1,
              s2vsb : 1,
              ola : 1,
              olb : 1,
              t120 : 1,
              t150 : 1;
        uint16_t : 16,
              cs_actual : 5,
                : 3;  
        bool : 7,
              stst : 1;

      };
    };
  };
  struct PWM_SCALE_t {
    constexpr static uint8_t address = 0x71;
    union {
      uint32_t sr;
      struct {
        uint8_t pwm_scale_sum : 8,
                : 8;
        int16_t pwm_scale_auto : 9;
      };
    };
  };
}


#pragma pack(pop)
