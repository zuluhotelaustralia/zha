// Constants (for GF[Get|Set]NextIndex)
/////////////////////////////////////////////////////////////////
const GFIDX_TEXT        := 0x01;	// Static Text (TextLine)
const GFIDX_TEXTENTRY   := 0x02;	// Return Text (TextEntry)
const GFIDX_BUTTON      := 0x03;	// Action Buttons (ButtonID)
const GFIDX_CHECKBOX    := 0x04;	// Check-boxes
const GFIDX_RADIO       := 0x05;	// Radio Buttons
/////////////////////////////////////////////////////////////////

// Constants for index starting values (SHOULD be enough! Heh!)
/////////////////////////////////////////////////////////////////
const GFINIT_TEXT       := 0x000;	// Static Text (GFdata[])
const GFINIT_TEXTENTRY  := 0x201;	// Return Text (GFdata[])
const GFINIT_BUTTON     := 0x401;	// Action Buttons (KeyIDs)
const GFINIT_CHECKBOX   := 0x601;	// Check-boxes (KeyIDs)
const GFINIT_RADIO      := 0x801;	// Radio Buttons (KeyIDs)
/////////////////////////////////////////////////////////////////

// 'gumpid' Constants for the GFResizePic() function
/////////////////////////////////////////////////////////////////
const BKGND_BLACKSLAB   := 0x0e10;  // Raised edge (blocky)
const BKGND_GOLDTRIM    := 0x0a3c;  // Gold-press, black backing
const BKGND_PAPER       := 0x0bb8;  // Very thin edging
const BKGND_PAPER_EX    := 0x0dac;  //  \_with fancy inner border
const BKGND_SCROLL      := 0x09d8;  // Unrolling scroll style
const BKGND_SILVERTRIM  := 0x0053;  // Fancy, dark-grey backing
const BKGND_STONESLAB   := 0x13be;  // Plain, small corners
const BKGND_STONE       := 0x13ec;  // Marble stone, rounded edge
const BKGND_OLD_JOURNAL_SCROLL  := 5150;  // Old journal scroll
const BKGND_DARK_STONE          := 5120;  // Same as previous, but much darker
///////////////////////////////////////////////////////////////
const BKGND_STONESLAB2                 := 9200; // transparent corners
const BKGND_SMOOTH_DARK_STONE          := 9250;
const BKGND_SMOOTH_DARK_STONE_BEIGE    := 9260;
const BKGND_BLACKHARD                  := 9270;
const BKGND_BEIGE_PAPER                := 9300;
const BKGND_WHITE_PAPER                := 9350;
const BKGND_BEIGE_SCROLL               := 9380;
const BKGND_WHITE_SCROLL               := 9390;
const BKGND_GRAY_GOLDROUNDED           := 9400;
const BKGND_GRAY_LIGHT_GOLDROUNDED     := 9450;
const BKGND_GRAY_PRESSED_GOLDROUNDED   := 9500;
const BKGND_GRAY_SILVERROUNDED         := 9550;
const BKGND_GRAY_LIGHT_SILVERROUNDED   := 9559;
const BKGND_GRAY_PRESSED_SILVERROUNDED := 9568;
/////////////////////////////////////////////////////////////////

// 'gumpid' Constants for the buttons or gumppics (All have a 0 and 1)
/////////////////////////////////////////////////////////////////
const G_BLACKGOLD_ARROW_UP_0	:= 0x00fa;
const G_BLACKGOLD_ARROW_UP_1	:= 0x00fb;
const G_BLACKGOLD_ARROW_DOWN_0	:= 0x00fc;
const G_BLACKGOLD_ARROW_DOWN_1	:= 0x00fd;
const G_BLUE_DIAMOND_0		:= 0x0868;
const G_BLUE_DIAMOND_1		:= 0x0867;
const G_BLUE_DIAMOND_CHECK_0	:= 0x086a;
const G_BLUE_DIAMOND_CHECK_1	:= 0x0869;
const G_BLUE_LARGE_CIRCLE_0	:= 0x0846;
const G_BLUE_LARGE_CIRCLE_1	:= 0x0845;
const G_BLUE_SMALL_CIRCLE_0	:= 0x0838;
const G_BLUE_SMALL_CIRCLE_1	:= 0x0837;
const G_BUTTON_ADD_0		:= 0x099c;
const G_BUTTON_ADD_1		:= 0x099d;
const G_BUTTON_APPLY_0		:= 0x084c;
const G_BUTTON_APPLY_1		:= 0x084b;
const G_BUTTON_CANCEL_0		:= 0x0849;
const G_BUTTON_CANCEL_1		:= 0x0848;
const G_BUTTON_DEFAULT_0	:= 0x084f;
const G_BUTTON_DEFAULT_1	:= 0x084e;
const G_BUTTON_DELETE_0		:= 0x099f;
const G_BUTTON_DELETE_1		:= 0x09a0;
const G_BUTTON_NEXT_0		:= 0x09a5;
const G_BUTTON_NEXT_1		:= 0x09a6;
const G_BUTTON_OKAY_0		:= 0x0852;
const G_BUTTON_OKAY_1		:= 0x0851;
const G_BUTTON_PREVIOUS_0	:= 0x09a2;
const G_BUTTON_PREVIOUS_1	:= 0x09a3;
const G_GOLD_ARROW_UP_0		:= 0x0984;
const G_GOLD_ARROW_UP_1		:= 0x0983;
const G_GOLD_ARROW_DOWN_0	:= 0x0985;
const G_GOLD_ARROW_DOWN_1	:= 0x0986;
const G_GOLDTRIM_MINUS_0	:= 0x0a54;
const G_GOLDTRIM_MINUS_1	:= 0x0a55;
const G_GOLDTRIM_X_0		:= 0x0a50;
const G_GOLDTRIM_X_1		:= 0x0a51;
const G_GRAY_CIRCLE_0		:= 0x00d0;
const G_GRAY_CIRCLE_1		:= 0x00d1;
const G_GREEN_ARROW_RIGHT_0	:= 0x15a4;
const G_GREEN_ARROW_RIGHT_1	:= 0x15a6;
const G_GREEN_SMALL_CIRCLE_1	:= 0x939;
const G_MARBLE_ARROW_DOWN_0	:= 0x0a58;
const G_MARBLE_ARROW_DOWN_1	:= 0x0a59;
const G_MARBLE_ARROW_UP_0	:= 0x0a5a;
const G_MARBLE_ARROW_UP_1	:= 0x0a5b;
const G_MARBLE_MINUS_0		:= 0x0a96;
const G_MARBLE_MINUS_1		:= 0x0a97;
const G_MARBLE_X_0		:= 0x0a94;
const G_MARBLE_X_1		:= 0x0a95;
const G_MINI_SCROLL_0		:= 0x0fc0;
const G_MINI_SCROLL_1		:= 0x0fc1;
const G_PARTY_0			:= 0x0fa8;
const G_PARTY_1			:= 0x0faa;
const G_PARTY_ARROW_LEFT_0	:= 0x0fae;
const G_PARTY_ARROW_LEFT_1	:= 0x0fb0;
const G_PARTY_ARROW_RIGHT_0	:= 0x0fa5;
const G_PARTY_ARROW_RIGHT_1	:= 0x0fa7;
const G_PARTY_CANCEL_0		:= 0x0fb4;
const G_PARTY_CANCEL_1		:= 0x0fb6;
const G_PARTY_E_0		:= 0x0fba;
const G_PARTY_E_1		:= 0x0fbc;
const G_PARTY_OK_0		:= 0x0fb7;
const G_PARTY_OK_1		:= 0x0fb9;
const G_PARTY_SAVE_0		:= 0x0fbd;
const G_PARTY_SAVE_1		:= 0x0fbf;
const G_PARTY_SCROLL_0		:= 0x0fab;
const G_PARTY_SCROLL_1		:= 0x0fad;
const G_PARTY_X_0		:= 0x0fb1;
const G_PARTY_X_1		:= 0x0fb3;
const G_PARTY_X_GUY_0		:= 0x0fa2;
const G_PARTY_X_GUY_1		:= 0x0fa4;
const G_RED_ARROW_LEFT_0	:= 0x15a1;
const G_RED_ARROW_LEFT_1	:= 0x15a3;
const G_RED_DIAMOND_0		:= 0x09a8;
const G_RED_DIAMOND_1		:= 0x09a9;
const G_RED_SMALL_CIRCLE_1 := 0x938;
const G_STONE_CHECK_0		:= 0x13ba;
const G_STONE_CHECK_1		:= 0x13bb;
const G_STONE_X_0		:= 0x13bc;
const G_STONE_X_1		:= 0x13bd;

// 'gumpid' Constants for the text areas
/////////////////////////////////////////////////////////////////
const G_TX_MARBLE_S_DK		:= 0x098c;
const G_TX_MARBLE_M		:= 0x098d;
const G_TX_MARBLE_SQ_L		:= 0x098e;
const G_TX_GOLDTRIM_LONG	:= 0x0476;
const G_TX_GOLDTRIM_LONG_LIGHT	:= 0x0477;
const G_TX_GOLDTRIM_LONG_DARK	:= 0x0475;

// 'gumpid' Constants for the checkboxes
/////////////////////////////////////////////////////////////////
const G_GRAY_CHECKBOX_0		:= 0x00d2;
const G_GRAY_CHECKBOX_1		:= 0x00d3;
const G_SMALL_SQUARE_CHECKBOX_0	:= 0x09ce;
const G_SMALL_SQUARE_CHECKBOX_1	:= 0x09cf;
const G_STONE_CHECKBOX_0	:= 0x146e;
const G_STONE_CHECKBOX_1	:= 0x146f;

// 'gumpid' Constants for the simple gumppics
/////////////////////////////////////////////////////////////////
const G_BOOK_LARGE_GOLDCORNER		:= 0x01f4;
const G_BOOK_LARGE_WOODCORNER		:= 0x01fe;
const G_BOOK_SMALL_1			:= 0x0898;
const G_BOOK_SMALL_2			:= 0x0899;
const G_BOOK_SMALL_3			:= 0x089a;
const G_BOOK_SMALL_4			:= 0x089b;
const G_BOOK_TITLE_GUMPPIC		:= 0x089c;
const G_BUYWIN_MINUS			:= 0x0038;
const G_BUYWIN_PLUS			:= 0x0037;
const G_CORNER_LEFT			:= 0x089d;
const G_CORNER_RIGHT			:= 0x089e;
const G_CORNER_GOLD_LEFT		:= 0x01f5;
const G_CORNER_GOLD_RIGHT		:= 0x01f6;
const G_CORNER_WOOD_LEFT		:= 0x01ff;
const G_CORNER_WOOD_RIGHT		:= 0x0200;
const G_DECO_LINE_LEFT			:= 0x0039;
const G_DECO_LINE_CENTER		:= 0x003a;
const G_DECO_LINE_RIGHT			:= 0x003b;
const G_GOLDEN_BOARD			:= 0x0064;
const G_GRAVESTONE_ROUND		:= 0x0065;
const G_GRAVESTONE_SQUARE		:= 0x0066;
const G_PAPERDOLL_OTHERS		:= 0x07d1;
const G_PAPERDOLL_SELF			:= 0x07d0;
const G_SKILLWIN_ARROW_DOWN_S		:= 0x0826;
const G_SKILLWIN_ARROW_RIGHT_S		:= 0x0827;
const G_SKILLWIN_LINE_L			:= 0x082b;
const G_SKILLWIN_LINE_S			:= 0x0835;
const G_SKILLWIN_LOCKER			:= 0x082c;
const G_SMALL_BLUE_CIRCLE		:= 0x093a;
const G_SMALL_GREEN_CIRCLE		:= 0x0939;
const G_SMALL_RED_CIRCLE		:= 0x0938;
const G_SPELLBOOK			:= 0x08ac;
const G_SPELLBOOK_CORNER_LEFT		:= 0x08bb;
const G_SPELLBOOK_CORNER_RIGHT		:= 0x08bc;
const G_SPELLBOOK_REDUCED		:= 0x08ba;
const G_SPELLCIRCLE_1			:= 0x08b1;
const G_SPELLCIRCLE_2			:= 0x08b2;
const G_SPELLCIRCLE_3			:= 0x08b3;
const G_SPELLCIRCLE_4			:= 0x08b4;
const G_SPELLCIRCLE_5			:= 0x08b5;
const G_SPELLCIRCLE_6			:= 0x08b6;
const G_SPELLCIRCLE_7			:= 0x08b7;
const G_SPELLCIRCLE_8			:= 0x08b8;
const G_SPELLICON_CLUMSY		:= 0x08c0;
const G_SPELLICON_CREATE_FOOD		:= 0x08c1;
const G_SPELLICON_FEEBLEMIND		:= 0x08c2;
const G_SPELLICON_HEAL			:= 0x08c3;
const G_SPELLICON_MAGIC_ARROW		:= 0x08c4;
const G_SPELLICON_NIGHTSIGHT		:= 0x08c5;
const G_SPELLICON_REACTIVE_ARMOR	:= 0x08c6;
const G_SPELLICON_WEAKEN		:= 0x08c7;
const G_SPELLICON_AGILITY		:= 0x08c8;
const G_SPELLICON_CUNNING		:= 0x08c9;
const G_SPELLICON_CURE			:= 0x08ca;
const G_SPELLICON_HARM			:= 0x08cb;
const G_SPELLICON_MAGIC_TRAP		:= 0x08cc;
const G_SPELLICON_MAGIC_UNTRAP		:= 0x08cd;
const G_SPELLICON_PROTECTION		:= 0x08ce;
const G_SPELLICON_STRENGTH		:= 0x08cf;
const G_SPELLICON_BLESS			:= 0x08d0;
const G_SPELLICON_FIREBALL		:= 0x08d1;
const G_SPELLICON_MAGIC_LOCK		:= 0x08d2;
const G_SPELLICON_POISON		:= 0x08d3;
const G_SPELLICON_TELEKINESIS		:= 0x08d4;
const G_SPELLICON_TELEPORT		:= 0x08d5;
const G_SPELLICON_UNLOCK		:= 0x08d6;
const G_SPELLICON_WALL_OF_STONE		:= 0x08d7;
const G_SPELLICON_ARCH_CURE		:= 0x08d8;
const G_SPELLICON_ARCH_PROTECTION	:= 0x08d9;
const G_SPELLICON_CURSE			:= 0x08da;
const G_SPELLICON_FIRE_FIELD		:= 0x08db;
const G_SPELLICON_GREATER_HEAL		:= 0x08dc;
const G_SPELLICON_LIGHTNING		:= 0x08dd;
const G_SPELLICON_MANA_DRAIN		:= 0x08de;
const G_SPELLICON_RECALL		:= 0x08df;
const G_SPELLICON_BLADE_SPIRIT		:= 0x08e0;
const G_SPELLICON_DISPEL_FIELD		:= 0x08e1;
const G_SPELLICON_INCOGNITO		:= 0x08e2;
const G_SPELLICON_MAGIC_REFLECTION	:= 0x08e3;
const G_SPELLICON_MIND_BLAST		:= 0x08e4;
const G_SPELLICON_PARALYZE		:= 0x08e5;
const G_SPELLICON_POISON_FIELD		:= 0x08e6;
const G_SPELLICON_SUMMON_CREATURE	:= 0x08e7;
const G_SPELLICON_DISPEL		:= 0x08e8;
const G_SPELLICON_ENERGY_BOLT		:= 0x08e9;
const G_SPELLICON_EXPLOSION		:= 0x08ea;
const G_SPELLICON_INVISIBILITY		:= 0x08eb;
const G_SPELLICON_MARK			:= 0x08ec;
const G_SPELLICON_MASS_CURSE		:= 0x08ed;
const G_SPELLICON_PARALYZE_FIELD	:= 0x08ee;
const G_SPELLICON_REVEAL		:= 0x08ef;
const G_SPELLICON_CHAIN_LIGHTNING	:= 0x08f0;
const G_SPELLICON_ENERGY_FIELD		:= 0x08f1;
const G_SPELLICON_FLAME_STRIKE		:= 0x08f2;
const G_SPELLICON_GATE_TRAVEL		:= 0x08f3;
const G_SPELLICON_MANA_VAMPIRE		:= 0x08f4;
const G_SPELLICON_MASS_DISPEL		:= 0x08f5;
const G_SPELLICON_METEOR_SWARM		:= 0x08f6;
const G_SPELLICON_POLYMORPH		:= 0x08f7;
const G_SPELLICON_EARTHQUAKE		:= 0x08f8;
const G_SPELLICON_ENERGY_VORTEX		:= 0x08f9;
const G_SPELLICON_RESURRECTION		:= 0x08fa;
const G_SPELLICON_SUMMON_AIR_ELEMENTAL	:= 0x08fb;
const G_SPELLICON_SUMMON_DAEMON		:= 0x08fc;
const G_SPELLICON_SUMMON_EARTH_ELEMENTAL:= 0x08fd;
const G_SPELLICON_SUMMON_FIRE_ELEMENTAL	:= 0x08fe;
const G_SPELLICON_SUMMON_WATER_ELEMENTAL:= 0x08ff;
const G_STONE_ARROW_UP			:= 0x1458;
const G_STONE_ARROW_DOWN		:= 0x1459;
const G_STONE_ARROW_LEFT		:= 0x1467;
const G_STONE_ARROW_RIGHT		:= 0x1468;


//Standard text colors on TSSE
/////////////////////////////////////////////////////////////////
const StdColor := 1890;
const HeaderColor := 1890;
const SelectedColor := 1720;
/////////////////////////////////////////////////////////////////

// CONSTants for the GFAddButton() function
CONST GF_PAGE_BTN	:= 0x0;
CONST GF_CLOSE_BTN	:= 0x1;
CONST GF_DUMMY_BTN	:= 0x2;