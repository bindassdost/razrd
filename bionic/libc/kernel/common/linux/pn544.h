/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#define PN544_MAGIC 0xE9
#define PN544_SET_PWR _IOW(PN544_MAGIC, 0x01, unsigned int)
//<2012/12/18 Yuting Shih, NFC SIM setting when initialization.
#define   PN544_SIM_SWP       _IOW(PN544_MAGIC, 0x02, unsigned int)
#define   PN544_GET_MODEL_ID  _IOW(PN544_MAGIC, 0x03, unsigned int)

#define   SWP_SIM1            0
#define   SWP_SIM2            1
//>2012/12/18 Yuting Shih.
struct pn544_i2c_platform_data {
 unsigned int irq_gpio;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 unsigned int ven_gpio;
 unsigned int firm_gpio;
};
