var layout := array(
"page 0",
"gumppic  10   0  255",
"gumppic   0   0  255",
"gumppic  20   0  255",
"button    3   3 4017 4019 0  1 0",

"page 1",
"gumppic   0   0  5100",
"gumppic   6   0  5101",
"gumppic  95   0  5101",
"gumppic 185   0  5101",
"gumppic 275   0  5102",
"gumppic   0   6  5103",
"gumppic   6   6  5104",
"gumppic  95   6  5104",
"gumppic 185   6  5104",
"gumppic 275   6  5105",
"gumppic   0  95  5103",
"gumppic   6  95  5104",
"gumppic  95  95  5104",
"gumppic 185  95  5104",
"gumppic 275  95  5105",
"gumppic   0 184  5103",
"gumppic   6 184  5104",
"gumppic  95 184  5104",
"gumppic 185 184  5104",
"gumppic 275 184  5105",
"gumppic   0 273  5106",
"gumppic   6 273  5107",
"gumppic  95 273  5107",
"gumppic 185 273  5107",
"gumppic 275 273  5108",
"gumppic  17  30  3001",
"text     75  10     0  0        ",             //GM Item Tools text
"text    190 255     0 11        ",             //cancel text
"button  240 253 4020 4022 1 0  0",             //cancel
"text     45  40     0 12        ",             //inferno trap text
"button    5  38 4023 4025 1 0 11",             //inferno trap
"text     45  61     0 23        ",             //explosion trap  text
"button    5  59 4023 4025 1 0 18",             //explosion trap
"text     45  82     0 24        ",             //poison trap text
"button    5  80 4023 4025 1 0 19",             //poison trap
"text     45 103     0 25        ",             //dart trap text
"button    5 101 4023 4025 1 0 20",             //dart trap
"text     45 145     0 28        ",             //hideinferno 5 text
"button    5 143 4023 4025 1 0 22",             //hideinferno 5
"text     45 166     0 29        ",             //hideinferno 10 text
"button    5 164 4023 4025 1 0 23",             //hideinferno 10
"text     45 187     0 30        ",             //showinferno 5 text
"button    5 185 4023 4025 1 0 24",             //showinferno 5
"text     45 208     0 31        ",             //showinferno 10 text
"button    5 206 4023 4025 1 0 25",             //showinferno 10
"text     45 229     0 27        ",             //remove trap text
"button    5 227 4020 4022 1 0 21"             //remove trap
);

var data := array(  
    "GM Trap Tools",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Close",                                   //11
    "Create Inferno Trap",                      //12
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Place Explosion Trap on an item",          //23
    "Place Poison Trap on an item",             //24
    "Place Dart Trap on an item",               //25
    "",
    "Remove Trap From Item",                    //27
    "Hide Infernos Range  5",                   //28
    "Hide Infernos Range 10",                   //29
    "Show Infernos Range  5",                   //30
    "Show Infernos Range 10"                  //31
);