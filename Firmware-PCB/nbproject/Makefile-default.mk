#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=config.c SPI_driver.c UART_driver.c LCD_driver.c I2C_driver.c HT16_leds_driver.c HT16_keys_driver.c EEPROM_driver.c SDCARD_driver.c encoders_driver.c bpm.c input_handler.c LCD_templates.c MIDI.c mode.c sequencer.c main.c init.c output_handler.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/config.o ${OBJECTDIR}/SPI_driver.o ${OBJECTDIR}/UART_driver.o ${OBJECTDIR}/LCD_driver.o ${OBJECTDIR}/I2C_driver.o ${OBJECTDIR}/HT16_leds_driver.o ${OBJECTDIR}/HT16_keys_driver.o ${OBJECTDIR}/EEPROM_driver.o ${OBJECTDIR}/SDCARD_driver.o ${OBJECTDIR}/encoders_driver.o ${OBJECTDIR}/bpm.o ${OBJECTDIR}/input_handler.o ${OBJECTDIR}/LCD_templates.o ${OBJECTDIR}/MIDI.o ${OBJECTDIR}/mode.o ${OBJECTDIR}/sequencer.o ${OBJECTDIR}/main.o ${OBJECTDIR}/init.o ${OBJECTDIR}/output_handler.o
POSSIBLE_DEPFILES=${OBJECTDIR}/config.o.d ${OBJECTDIR}/SPI_driver.o.d ${OBJECTDIR}/UART_driver.o.d ${OBJECTDIR}/LCD_driver.o.d ${OBJECTDIR}/I2C_driver.o.d ${OBJECTDIR}/HT16_leds_driver.o.d ${OBJECTDIR}/HT16_keys_driver.o.d ${OBJECTDIR}/EEPROM_driver.o.d ${OBJECTDIR}/SDCARD_driver.o.d ${OBJECTDIR}/encoders_driver.o.d ${OBJECTDIR}/bpm.o.d ${OBJECTDIR}/input_handler.o.d ${OBJECTDIR}/LCD_templates.o.d ${OBJECTDIR}/MIDI.o.d ${OBJECTDIR}/mode.o.d ${OBJECTDIR}/sequencer.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/init.o.d ${OBJECTDIR}/output_handler.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/config.o ${OBJECTDIR}/SPI_driver.o ${OBJECTDIR}/UART_driver.o ${OBJECTDIR}/LCD_driver.o ${OBJECTDIR}/I2C_driver.o ${OBJECTDIR}/HT16_leds_driver.o ${OBJECTDIR}/HT16_keys_driver.o ${OBJECTDIR}/EEPROM_driver.o ${OBJECTDIR}/SDCARD_driver.o ${OBJECTDIR}/encoders_driver.o ${OBJECTDIR}/bpm.o ${OBJECTDIR}/input_handler.o ${OBJECTDIR}/LCD_templates.o ${OBJECTDIR}/MIDI.o ${OBJECTDIR}/mode.o ${OBJECTDIR}/sequencer.o ${OBJECTDIR}/main.o ${OBJECTDIR}/init.o ${OBJECTDIR}/output_handler.o

# Source Files
SOURCEFILES=config.c SPI_driver.c UART_driver.c LCD_driver.c I2C_driver.c HT16_leds_driver.c HT16_keys_driver.c EEPROM_driver.c SDCARD_driver.c encoders_driver.c bpm.c input_handler.c LCD_templates.c MIDI.c mode.c sequencer.c main.c init.c output_handler.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX150F128B
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config.o.d 
	@${RM} ${OBJECTDIR}/config.o 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/config.o.d" -o ${OBJECTDIR}/config.o config.c   
	
${OBJECTDIR}/SPI_driver.o: SPI_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI_driver.o.d 
	@${RM} ${OBJECTDIR}/SPI_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_driver.o.d" -o ${OBJECTDIR}/SPI_driver.o SPI_driver.c   
	
${OBJECTDIR}/UART_driver.o: UART_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART_driver.o.d 
	@${RM} ${OBJECTDIR}/UART_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/UART_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/UART_driver.o.d" -o ${OBJECTDIR}/UART_driver.o UART_driver.c   
	
${OBJECTDIR}/LCD_driver.o: LCD_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD_driver.o.d 
	@${RM} ${OBJECTDIR}/LCD_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LCD_driver.o.d" -o ${OBJECTDIR}/LCD_driver.o LCD_driver.c   
	
${OBJECTDIR}/I2C_driver.o: I2C_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2C_driver.o.d 
	@${RM} ${OBJECTDIR}/I2C_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/I2C_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/I2C_driver.o.d" -o ${OBJECTDIR}/I2C_driver.o I2C_driver.c   
	
${OBJECTDIR}/HT16_leds_driver.o: HT16_leds_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HT16_leds_driver.o.d 
	@${RM} ${OBJECTDIR}/HT16_leds_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/HT16_leds_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/HT16_leds_driver.o.d" -o ${OBJECTDIR}/HT16_leds_driver.o HT16_leds_driver.c   
	
${OBJECTDIR}/HT16_keys_driver.o: HT16_keys_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HT16_keys_driver.o.d 
	@${RM} ${OBJECTDIR}/HT16_keys_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/HT16_keys_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/HT16_keys_driver.o.d" -o ${OBJECTDIR}/HT16_keys_driver.o HT16_keys_driver.c   
	
${OBJECTDIR}/EEPROM_driver.o: EEPROM_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM_driver.o.d 
	@${RM} ${OBJECTDIR}/EEPROM_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/EEPROM_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/EEPROM_driver.o.d" -o ${OBJECTDIR}/EEPROM_driver.o EEPROM_driver.c   
	
${OBJECTDIR}/SDCARD_driver.o: SDCARD_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SDCARD_driver.o.d 
	@${RM} ${OBJECTDIR}/SDCARD_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/SDCARD_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SDCARD_driver.o.d" -o ${OBJECTDIR}/SDCARD_driver.o SDCARD_driver.c   
	
${OBJECTDIR}/encoders_driver.o: encoders_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoders_driver.o.d 
	@${RM} ${OBJECTDIR}/encoders_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/encoders_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/encoders_driver.o.d" -o ${OBJECTDIR}/encoders_driver.o encoders_driver.c   
	
${OBJECTDIR}/bpm.o: bpm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/bpm.o.d 
	@${RM} ${OBJECTDIR}/bpm.o 
	@${FIXDEPS} "${OBJECTDIR}/bpm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/bpm.o.d" -o ${OBJECTDIR}/bpm.o bpm.c   
	
${OBJECTDIR}/input_handler.o: input_handler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/input_handler.o.d 
	@${RM} ${OBJECTDIR}/input_handler.o 
	@${FIXDEPS} "${OBJECTDIR}/input_handler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/input_handler.o.d" -o ${OBJECTDIR}/input_handler.o input_handler.c   
	
${OBJECTDIR}/LCD_templates.o: LCD_templates.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD_templates.o.d 
	@${RM} ${OBJECTDIR}/LCD_templates.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD_templates.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LCD_templates.o.d" -o ${OBJECTDIR}/LCD_templates.o LCD_templates.c   
	
${OBJECTDIR}/MIDI.o: MIDI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MIDI.o.d 
	@${RM} ${OBJECTDIR}/MIDI.o 
	@${FIXDEPS} "${OBJECTDIR}/MIDI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MIDI.o.d" -o ${OBJECTDIR}/MIDI.o MIDI.c   
	
${OBJECTDIR}/mode.o: mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mode.o.d 
	@${RM} ${OBJECTDIR}/mode.o 
	@${FIXDEPS} "${OBJECTDIR}/mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mode.o.d" -o ${OBJECTDIR}/mode.o mode.c   
	
${OBJECTDIR}/sequencer.o: sequencer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sequencer.o.d 
	@${RM} ${OBJECTDIR}/sequencer.o 
	@${FIXDEPS} "${OBJECTDIR}/sequencer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/sequencer.o.d" -o ${OBJECTDIR}/sequencer.o sequencer.c   
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c   
	
${OBJECTDIR}/init.o: init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/init.o.d 
	@${RM} ${OBJECTDIR}/init.o 
	@${FIXDEPS} "${OBJECTDIR}/init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init.o.d" -o ${OBJECTDIR}/init.o init.c   
	
${OBJECTDIR}/output_handler.o: output_handler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/output_handler.o.d 
	@${RM} ${OBJECTDIR}/output_handler.o 
	@${FIXDEPS} "${OBJECTDIR}/output_handler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/output_handler.o.d" -o ${OBJECTDIR}/output_handler.o output_handler.c   
	
else
${OBJECTDIR}/config.o: config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config.o.d 
	@${RM} ${OBJECTDIR}/config.o 
	@${FIXDEPS} "${OBJECTDIR}/config.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/config.o.d" -o ${OBJECTDIR}/config.o config.c   
	
${OBJECTDIR}/SPI_driver.o: SPI_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI_driver.o.d 
	@${RM} ${OBJECTDIR}/SPI_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_driver.o.d" -o ${OBJECTDIR}/SPI_driver.o SPI_driver.c   
	
${OBJECTDIR}/UART_driver.o: UART_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UART_driver.o.d 
	@${RM} ${OBJECTDIR}/UART_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/UART_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/UART_driver.o.d" -o ${OBJECTDIR}/UART_driver.o UART_driver.c   
	
${OBJECTDIR}/LCD_driver.o: LCD_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD_driver.o.d 
	@${RM} ${OBJECTDIR}/LCD_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LCD_driver.o.d" -o ${OBJECTDIR}/LCD_driver.o LCD_driver.c   
	
${OBJECTDIR}/I2C_driver.o: I2C_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/I2C_driver.o.d 
	@${RM} ${OBJECTDIR}/I2C_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/I2C_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/I2C_driver.o.d" -o ${OBJECTDIR}/I2C_driver.o I2C_driver.c   
	
${OBJECTDIR}/HT16_leds_driver.o: HT16_leds_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HT16_leds_driver.o.d 
	@${RM} ${OBJECTDIR}/HT16_leds_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/HT16_leds_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/HT16_leds_driver.o.d" -o ${OBJECTDIR}/HT16_leds_driver.o HT16_leds_driver.c   
	
${OBJECTDIR}/HT16_keys_driver.o: HT16_keys_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HT16_keys_driver.o.d 
	@${RM} ${OBJECTDIR}/HT16_keys_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/HT16_keys_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/HT16_keys_driver.o.d" -o ${OBJECTDIR}/HT16_keys_driver.o HT16_keys_driver.c   
	
${OBJECTDIR}/EEPROM_driver.o: EEPROM_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM_driver.o.d 
	@${RM} ${OBJECTDIR}/EEPROM_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/EEPROM_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/EEPROM_driver.o.d" -o ${OBJECTDIR}/EEPROM_driver.o EEPROM_driver.c   
	
${OBJECTDIR}/SDCARD_driver.o: SDCARD_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SDCARD_driver.o.d 
	@${RM} ${OBJECTDIR}/SDCARD_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/SDCARD_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SDCARD_driver.o.d" -o ${OBJECTDIR}/SDCARD_driver.o SDCARD_driver.c   
	
${OBJECTDIR}/encoders_driver.o: encoders_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/encoders_driver.o.d 
	@${RM} ${OBJECTDIR}/encoders_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/encoders_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/encoders_driver.o.d" -o ${OBJECTDIR}/encoders_driver.o encoders_driver.c   
	
${OBJECTDIR}/bpm.o: bpm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/bpm.o.d 
	@${RM} ${OBJECTDIR}/bpm.o 
	@${FIXDEPS} "${OBJECTDIR}/bpm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/bpm.o.d" -o ${OBJECTDIR}/bpm.o bpm.c   
	
${OBJECTDIR}/input_handler.o: input_handler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/input_handler.o.d 
	@${RM} ${OBJECTDIR}/input_handler.o 
	@${FIXDEPS} "${OBJECTDIR}/input_handler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/input_handler.o.d" -o ${OBJECTDIR}/input_handler.o input_handler.c   
	
${OBJECTDIR}/LCD_templates.o: LCD_templates.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LCD_templates.o.d 
	@${RM} ${OBJECTDIR}/LCD_templates.o 
	@${FIXDEPS} "${OBJECTDIR}/LCD_templates.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LCD_templates.o.d" -o ${OBJECTDIR}/LCD_templates.o LCD_templates.c   
	
${OBJECTDIR}/MIDI.o: MIDI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MIDI.o.d 
	@${RM} ${OBJECTDIR}/MIDI.o 
	@${FIXDEPS} "${OBJECTDIR}/MIDI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/MIDI.o.d" -o ${OBJECTDIR}/MIDI.o MIDI.c   
	
${OBJECTDIR}/mode.o: mode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mode.o.d 
	@${RM} ${OBJECTDIR}/mode.o 
	@${FIXDEPS} "${OBJECTDIR}/mode.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mode.o.d" -o ${OBJECTDIR}/mode.o mode.c   
	
${OBJECTDIR}/sequencer.o: sequencer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/sequencer.o.d 
	@${RM} ${OBJECTDIR}/sequencer.o 
	@${FIXDEPS} "${OBJECTDIR}/sequencer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/sequencer.o.d" -o ${OBJECTDIR}/sequencer.o sequencer.c   
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c   
	
${OBJECTDIR}/init.o: init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/init.o.d 
	@${RM} ${OBJECTDIR}/init.o 
	@${FIXDEPS} "${OBJECTDIR}/init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init.o.d" -o ${OBJECTDIR}/init.o init.c   
	
${OBJECTDIR}/output_handler.o: output_handler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/output_handler.o.d 
	@${RM} ${OBJECTDIR}/output_handler.o 
	@${FIXDEPS} "${OBJECTDIR}/output_handler.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/output_handler.o.d" -o ${OBJECTDIR}/output_handler.o output_handler.c   
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_ICD3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}           -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC00490:0x1FC00BEF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Firmware-PCB.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
