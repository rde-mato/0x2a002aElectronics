#include "0x2a002a.h"

enum E_MIDI_STATUS
{
    /* OR'd with channel number 0 indexed */
    /* Channel Voice Messages */    /* data bytes */
    E_MS_NOTE_OFF = 0x80,           //      x2
    E_MS_NOTE_ON = 0x90,            //      x2      (velocity 0 == note off)
    E_MS_POLY_KEY_PRESSURE = 0xA0,  //      x2
    E_MS_CONTROL_CHANGE = 0xB0,     //      x2
    E_MS_PROGRAM_CHANGE = 0xC0,     //      x1
    E_MS_CHANNEL_PRESSURE = 0xD0,   //      x1
    E_MS_PITCH_BEND_CHANGE = 0xE0,  //      x2

    /* OR'd with channel number 0 indexed */
    /* Channel Mode Messages */     /* data bytes */
    E_MS_CHANNEL_MODE = 0xB0,       // 0x78 | X  + 1    //XXX: point noir a clarifier.


    /* System Messages */   /* OR'd with */ /* data bytes */   /* description */
    E_MS_SYSTEM = 0xF0,     //  0b0000      ID(0 - 127) + ...  System Exclusive
    //////////////////,     //  0b0sss      ?(1 - 7) + x(0 - 2)  System Common
    //////////////////,     //  0b1ttt         ?(0 - 7)        System Real Time
};

static u8  running_status;

/*
** Sets MIDI Note Off.
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** velocity: 0-127 MIDI Velocity
*/
void midi_note_off(u8 channel, u8 note, u8 velocity)
{
    u8  status;
    
    status = E_MS_NOTE_OFF | channel;
    if (velocity == 0 && running_status == E_MS_NOTE_ON | channel)
        ;
    else if (status != running_status)
    {
        UART1_send(status);
        running_status = status;
    }
    UART1_send(note);
    UART1_send(velocity);
}

/*
** Sets MIDI Note On.
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** velocity: 0-127 MIDI Velocity. If 0 => Note Off.
*/
void midi_note_on(u8 channel, u8 note, u8 velocity)
{
    u8 status;

    status = E_MS_NOTE_ON | channel;
    if (status != running_status)
    {
        UART1_send(status);
        running_status = status;
    }
    UART1_send(note);
    UART1_send(velocity);
}

/*
** ???
** channel: 0-15 MIDI Channel Number
** note: 0-127 MIDI Note
** value: 0-127 Pressure Value
*/
void midi_polyphonic_key_pressure(u8 channel, u8 note, u8 value)
{

}

/*
** Changes MIDI Channel Controller Value
** channel: 0-15 MIDI Channel Number
** control: 0-119 MIDI Control
** velocity: 0-127 Control Value
*/
void midi_control_change(u8 channel, u8 control, u8 value)
{

}

/*
** Changes MIDI Channel Program
** channel: 0-15 MIDI Channel Number
** program: 0-127 MIDI Program Number
*/
void midi_program_change(u8 channel, u8 program)
{

}

/*
** Changes Channel Pressure (Aftertouch)
** channel: 0-15 MIDI Channel Number
** value: 0-127 MIDI Pressure Value
*/
void midi_channel_pressure(u8 channel, u8 value)
{

}

/*
** Changes Pitch Bend
** channel: 0-15 MIDI Channel Number
** value: 0-127 Pitch Bend Change Value
*/
void midi_pitch_bend_change(u8 channel)
{
}


// General MIDI 1 instruments only.
// All channels except 10 ( reserved for drums )
enum E_GM_SOUND_SET {
    // 1-8 Piano
    GMSS_ACOUSTIC_GRAND_PIANO = 1,
    GMSS_BRIGHT_ACOUSTIC_PIANO = 2,
    GMSS_ELECTRIC_GRAND_PIANO = 3,
    GMSS_HONKY_TONK_PIANO = 4,
    GMSS_ELECTRIC_PIANO_1 = 5,
    GMSS_ELECTRIC_PIANO_2 = 6,
    GMSS_HARPSICHORD = 7,
    GMSS_CLAVI = 8,
    // 9-16 Chromatic Percussion
    GMSS_CELESTA = 9,
    GMSS_GLOCKENSPIEL = 10,
    GMSS_MUSIC_BOX = 11,
    GMSS_VIBRAPHONE = 12,
    GMSS_MARIMBA = 13,
    GMSS_XYLOPHONE = 14,
    GMSS_TUBULAR_BELLS = 15,
    GMSS_DULCIMER = 16,
    GMSS_DRAWBAR_ORGAN = 17,
    // 17-24 Organ
    GMSS_PERCUSSIVE_ORGAN = 18,
    GMSS_ROCK_ORGAN = 19,
    GMSS_CHURCH_ORGAN = 20,
    GMSS_REED_ORGAN = 21,
    GMSS_ACCORDION = 22,
    GMSS_HARMONICA = 23,
    GMSS_TANGO_ACCORDION = 24,
    // 25-32 Guitar
    GMSS_ACOUSTIC_GUITAR_NYLON = 25,
    GMSS_ACOUSTIC_GUITAR_STEEL = 26,
    GMSS_ELECTRIC_GUITAR_JAZZ = 27,
    GMSS_ELECTRIC_GUITAR_CLEAN = 28,
    GMSS_ELECTRIC_GUITAR_MUTED = 29,
    GMSS_OVERDRIVEN_GUITAR = 30,
    GMSS_DISTORTION_GUITAR = 31,
    GMSS_GUITAR_HARMONICS = 32,
    // 33-40 Bass
    GMSS_ACOUSTIC_BASS = 33,
    GMSS_ELECTRIC_BASS_FINGER = 34,
    GMSS_ELECTRIC_BASS_PICK = 35,
    GMSS_FRETLESS_BASS = 36,
    GMSS_SLAP_BASS_1 = 37,
    GMSS_SLAP_BASS_2 = 38,
    GMSS_SYNTH_BASS_1 = 39,
    GMSS_SYNTH_BASS_2 = 40,
    // 41-48 Strings
    GMSS_VIOLIN = 41,
    GMSS_VIOLA = 42,
    GMSS_CELLO = 43,
    GMSS_CONTRABASS = 44,
    GMSS_TREMOLO_STRINGS = 45,
    GMSS_PIZZICATO_STRINGS = 46,
    GMSS_ORCHESTRAL_HARP = 47,
    GMSS_TIMPANI = 48,
    // 49-56 Ensemble
    GMSS_STRING_ENSEMBLE_1 = 49,
    GMSS_STRING_ENSEMBLE_2 = 50,
    GMSS_SYNTH_STRINGS_1 = 51,
    GMSS_SYNTH_STRINGS_2 = 52,
    GMSS_CHOIR_AAHS = 53,
    GMSS_VOICE_OOHS = 54,
    GMSS_SYNTH_VOICE = 55,
    GMSS_ORCHESTRA_HIT = 56,
    // 57-64 Brass
    GMSS_TRUMPET = 57,
    GMSS_TRUMBONE = 58,
    GMSS_TUBA = 59,
    GMSS_MUTED_TRUMPET = 60,
    GMSS_FRENCH_HORN = 61,
    GMSS_BRASS_SECTION = 62,
    GMSS_SYNTH_BRASS_1 = 63,
    GMSS_SYNTH_BRASS_2 = 64,
    // 65-72 Reed
    GMSS_SOPRANO_SAX = 65,
    GMSS_ALTO_SAX = 66,
    GMSS_TENOR_SAX = 67,
    GMSS_BARINTONE_SAX = 68,
    GMSS_OBOE = 69,
    GMSS_ENGLISH_HORN = 70,
    GMSS_BASSOON = 71,
    GMSS_CLARINET = 72,
    // 73-80 Pipe
    GMSS_PICCOLO = 73,
    GMSS_FLUTE = 74,
    GMSS_RECORDER = 75,
    GMSS_PAN_FLUTE = 76,
    GMSS_BLOWN_BOTTLE = 77,
    GMSS_SHAKUHACHI = 78,
    GMSS_WHISTLE = 79,
    GMSS_OCARINA = 80,
    // 81-88 Synth Lead
    GMSS_LEAD_1_SQUARE = 81,
    GMSS_LEAD_2_SAWTOOTH = 82,
    GMSS_LEAD_3_CALLIOPE = 83,
    GMSS_LEAD_4_CHIFF = 84,
    GMSS_LEAD_5_CHARANG = 85,
    GMSS_LEAD_6_VOICE = 86,
    GMSS_LEAD_7_FIFTHS = 87,
    GMSS_LEAD_8_BASSLEAD = 88,
    // 89-96 Synth Pad
    GMSS_PAD_1_NEW_AGE = 89,
    GMSS_PAD_2_WARM = 90,
    GMSS_PAD_3_POLYSYNTH = 91,
    GMSS_PAD_4_CHOIR = 92,
    GMSS_PAD_5_BOWED = 93,
    GMSS_PAD_6_METALLIC = 94,
    GMSS_PAD_7_HALO = 95,
    GMSS_PAD_8_SWEEP = 96,
    // 97-104 Synth Effects
    GMSS_FX_1_RAIN = 97,
    GMSS_FX_2_SOUNDTRACK = 98,
    GMSS_FX_3_CRYSTAL = 99,
    GMSS_FX_4_ATMOSPHERE = 100,
    GMSS_FX_5_BRIGHTNESS = 101,
    GMSS_FX_6_GOBLINS = 102,
    GMSS_FX_7_ECHOES = 103,
    GMSS_FX_8_SCI_FI = 104,
    // 105-112 Ethnic
    GMSS_SITAR = 105,
    GMSS_BANJO = 106,
    GMSS_SHAMISEN = 107,
    GMSS_KOTO = 108,
    GMSS_KALIMBA = 109,
    GMSS_BAG_PIPE = 110,
    GMSS_FIDDLE = 111,
    GMSS_SHANAI = 112,
    // 113-120 Percussive
    GMSS_TINKLE_BELL = 113,
    GMSS_AGOGO = 114,
    GMSS_STEEL_DRUMS = 115,
    GMSS_WOODBLOCK = 116,
    GMSS_TAIKO_DRUM = 117,
    GMSS_MELODIC_TOM = 118,
    GMSS_SYNTH_DRUM = 119,
    GMSS_REVERSE_CYMBAL = 120,
    // 121-128 Sound Effects
    GMSS_GUITAR_FRET_NOISE = 121,
    GMSS_BREATH_NOISE = 122,
    GMSS_SEASHORE = 123,
    GMSS_BIRD_TWEET = 124,
    GMSS_TELEPHONE_RING = 125,
    GMSS_HELICOPTER = 126,
    GMSS_APPLAUSE = 127,
    GMSS_GUNSHOT = 128
};

// Notes on channel 10
enum E_GM_PERCUSSION_MAP {
    GMPM_ACOUSTIC_BASS_DRUM = 35,
    GMPM_BASS_DRUM_1 = 36,
    GMPM_SIDE_STICK = 37,
    GMPM_ACOUSTIC_SNARE = 38,
    GMPM_HAND_CLAP = 39,
    GMPM_ELECTRIC_SNARE = 40,
    GMPM_LOW_FLOOR_TOM = 41,
    GMPM_CLOSED_HI_HAT = 42,
    GMPM_HIGH_FLOOR_TOM = 43,
    GMPM_PEDAL_HI_HAT = 44,
    GMPM_LOW_TOM = 45,
    GMPM_OPEN_HI_HAT = 46,
    GMPM_LOW_MID_TOM = 47,
    GMPM_HI_MID_TOM = 48,
    GMPM_CRASH_CYMBAL_1 = 49,
    GMPM_HIGH_TOM = 50,
    GMPM_RIDE_CYMBAL_1 = 51,
    GMPM_CHINESE_CYMBAL = 52,
    GMPM_RIDE_BELL = 53,
    GMPM_TAMBOURINE = 54,
    GMPM_SPLASH_CYMBAL = 55,
    GMPM_COWBELL = 56,
    GMPM_CRASH_CYMBAL_2 = 57,
    GMPM_VIBRASLAP = 58,
    GMPM_RIDE_CYMBAL_2 = 59,
    GMPM_HI_BONGO = 60,
    GMPM_LOW_BONGO = 61,
    GMPM_MUTE_HI_CONGA = 62,
    GMPM_OPEN_HI_CONGA = 63,
    GMPM_LOW_CONGA = 64,
    GMPM_HIGH_TIMBALE = 65,
    GMPM_LOW_TIMBALE = 66,
    GMPM_HIGH_AGOGO = 67,
    GMPM_LOW_AGOGO = 68,
    GMPM_CABASA = 69,
    GMPM_MARACAS = 70,
    GMPM_SHORT_WHISTLE = 71,
    GMPM_LONG_WHISTLE = 72,
    GMPM_SHORT_GUIRO = 73,
    GMPM_LONG_GUIRO = 74,
    GMPM_CLAVES = 75,
    GMPM_HI_WOOD_BLOCK = 76,
    GMPM_LOW_WOOD_BLOCK = 77,
    GMPM_MUTE_CUICA = 78,
    GMPM_OPEN_CUICA = 79,
    GMPM_MUTE_TRIANGLE = 80,
    GMPM_OPEN_TRIANGLE = 81,
};

enum E_MIDI_CHANNEL_MESSAGES
{
    MCM_NOTE_OFF = 0x80,
    MCM_NOTE_ON = 0x90,
    MCM_POLYPHONIC_AFTERTOUCH = 0xA0,
    MCM_CONTROL_CHANGE = 0xB0,
    MCM_PROGRAM_CHANGE = 0xC0,
    MCM_CHANNEL_AFTERTOUCH = 0xD0,
    MCM_PICH_WHEEL = 0xE0,
};

enum E_MIDI_CONTROLLERS
{
    MC_BANK_SELECT = 0x00,
    MC_MODULATION_WHEEL = 0x01,
    MC_BREATH_CONTROLLER = 0x02,
    MC_FOOT_CONTROLLER = 0x04,
    MC_PORTAMENTO_TIME = 0x05,
    MC_DATA_ENTRY_SLIDER = 0x06,
    MC_MAIN_VOLUME = 0x07,
    MC_BALANCE = 0x08,
    MC_PAN = 0x0A,
    MC_EXPR_CONTROLLER = 0x0B,
    MC_FX_CONTROL_1 = 0x0C,
    MC_FX_CONTROL_2 = 0x0D,
    MC_GP_CONTROLLER_1 = 0x10,
    MC_GP_CONTROLLER_2 = 0x11,
    MC_GP_CONTROLLER_3 = 0x12,
    MC_GP_CONTROLLER_4 = 0x13,
    MC_SUSTAIN_PEDAL = 0x40,
    MC_PORTAMENTO_ON_OFF = 0x41,
    MC_SOSTENUTO_PEDAL = 0x42,
    MC_SOFT_PEDAL = 0x43,
    MC_LEGATO_FOOTSWITCH = 0x44,
    MC_HOLD_2 = 0x45,
    MC_SOUND_VARIATION = 0x46,
    MC_TIMBRE_CONTENT = 0x47,
    MC_RELEASE_TIME = 0x48,
    MC_ATTACK_TIME = 0x49,
    MC_BRIGHTNESS = 0x4A,
    MC_SOUND_CONTROLLER_6 = 0x4B,
    MC_SOUND_CONTROLLER_7 = 0x4C,
    MC_SOUND_CONTROLLER_8 = 0x4D,
    MC_SOUND_CONTROLLER_9 = 0x4E,
    MC_SOUND_CONTROLLER_10 = 0x4F,
    MC_GP_CONTROLLER_5 = 0x50,
    MC_GP_CONTROLLER_6 = 0x51,
    MC_GP_CONTROLLER_7 = 0x52,
    MC_GP_CONTROLLER_8 = 0x53,
    MC_PORTAMENTO_CONTROL = 0x54,
    MC_EXTERNAL_FX_DEPTH_1 = 0x5B,
    MC_TREMOLO_DEPTH_2 = 0x5C,
    MC_CHORUS_DEPTH_3 = 0x5D,
    MC_DETUNE_DEPTH_4 = 0x5E,
    MC_PHASE_DEPTH_5 = 0x5F,
    MC_DATA_INCREMENT = 0x60,
    MC_DATA_DECREMENT = 0x61,
};

enum E_MIDI_CHANNEL_MODE_MSG            // ARGUMENT
{
    MCMM_ALL_SOUNDS_OFF = 0x78,         // 0x00
    MCMM_RESET_ALL_CONTROLLERS = 0x79,  // 0x7F
    MCMM_LOCAL_CONTROL = 0x7A,          // 7F on, 00 off
    MCMM_ALL_NOTES_OFF = 0x7B,          // 0x00
    MCMM_OMNI_MODE_OFF = 0x7C,          // 0x00
    MCMM_OMNI_MODE_ON = 0x7D,           // 0x00
    MCMM_MONO_MODE_ON = 0x7E,           // channels ?? v
    MCMM_MONO_MODE_OFF = 0x7F,          // 00 ?? ^
};

void    midi_gm_on(void)
{
    UART1_send(0xF0);
    UART1_send(0x7E);
    UART1_send(0x7F);
    UART1_send(0x09);
    UART1_send(0x01);
    UART1_send(0xF7);
}

void    midi_gm_off(void)
{
    UART1_send(0xF0);
    UART1_send(0x7E);
    UART1_send(0x7F);
    UART1_send(0x09);
    UART1_send(0x02);
    UART1_send(0xF7);
}

void    midi_send_control_change(u8 channel, u8 function, u8 value)
{
    UART1_send(0xB0 | channel);
    UART1_send(function);
    UART1_send(value);
}