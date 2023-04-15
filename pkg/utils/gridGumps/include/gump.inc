use uo;

include ":gumps:gumps";
include ":gumps:gumps_ex";

var STYLE_FILE := ReadConfigFile(":gridGumps:styles");
const DEFAULT_STYLE := "GreyOnBlue";

function grid_Init( columns, rows, style:=DEFAULT_STYLE, ColumnWidth := 250, RowHeight := 30, x:=10, y:=10 )

	var settings := STYLE_FILE[style];
	var TotalWidth := (columns * ColumnWidth)+(settings.BackgroundXPadding*2);
	var TotalHeight := (rows * RowHeight)+(settings.BackgroundYPadding*2);

	var gump := GFCreateGump(x, y);
	GFResizePic( gump, x, y, GFGetCfgConst("BackGrounds", settings.Background), TotalWidth, TotalHeight );

	return struct{
	"gump":=gump,
	"settings":=settings,
	"columns":=columns,
	"rows":=rows,
	"ColumnWidth":=ColumnWidth,
	"RowHeight":=RowHeight
	};//returned

endfunction

function grid_AddTile( byref GridStruct, column, row, xSpan := 1, ySpan := 1 )

	var x := GridStruct.settings.BackgroundXPadding + ((column-1)*GridStruct.ColumnWidth) + GridStruct.gump.base.x;
	var y := GridStruct.settings.BackgroundYPadding + ((row-1)*GridStruct.RowHeight) + GridStruct.gump.base.y;
	GFResizePic(
		GridStruct.gump, /*gumpref*/
		x,  /*x*/
		y,  /*y*/
		GFGetCfgConst("BackGrounds", GridStruct.settings.TileBackground),  /*tile*/
		GridStruct.ColumnWidth*xSpan,   /*width*/
		GridStruct.RowHeight*ySpan);   /*heigh*/
	return struct{
	"column" := column,
	"row" := row,
	"x" := GridStruct.settings.TileXPadding + x,
	"y" := GridStruct.settings.TileYPadding + y,
	"width" := GridStruct.ColumnWidth*xSpan,
	"height" := GridStruct.RowHeight*ySpan,
	"endx" := x + (GridStruct.ColumnWidth*xSpan) - GridStruct.settings.TileXPadding,
	"endy" :=  y + (GridStruct.RowHeight*ySpan) - GridStruct.settings.TileYPadding
	};

endfunction
