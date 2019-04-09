/**
 * Flare Director
 * ASM
 * ASMToggles.cpp
 * 
 * This file contains the struct definition for the
 * the toggles needed to control various aspects of
 * the ASM software.
 */

typedef struct asm_toggles {
    bool reportingToggle;
    bool dataLoggingToggle;
    bool hasBeenCalibrated;
    bool ledFlashing;
} ASMToggles;
