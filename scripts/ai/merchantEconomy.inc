use datafile;
use math;
use cfgfile;



const FILEPATH := "::merchantEconomy";

function GetExemptObjTypes()

	return {0x0f3f, //arrows
		 0x6051, //firearrows
		 0xb200, //icearrows	
		 0x01bfb, //bolts
		 0x1bd4, //shafts
		 0x1bd1, //feathers
		 0xf7a, //blackpearl
		 0xf86, //mandrakeroot
		 0xf8c, //sulfourous ash
		 0xf84, //garlic
		 0xf8d, //s silk
		 0xf7b, //b moss
		 0xf88, //ns
		 0xf85 //ginger
 	};

endfunction

function IsInExemptObjTypes(objtype)
	foreach ex_obj_type in GetExemptObjTypes()
		if (objtype==Hex(ex_obj_type))
			return 1;
		endif
	endforeach
	return 0;
endfunction

function GetMerchantEconomyFile()
	var file := OpenDataFile( FILEPATH );
	if (!file || file ==error)
		CreateDataFile( FILEPATH);
	endif
	return file;

endfunction

function WriteSoldAmountsBasedOnInventory( merchanttype, inventory )
	//Broadcast("writing sold amounts...");
   foreach productgroup in ProductGroups(merchanttype)
		  //mod by barny to allow for economy system
		  //calculate how many of each item has been sold and write this to economy file
		  var pg_elem := FindConfigElem( mrcspawn_cfg, productgroup );
   		var obj_type, itempspawnamount, iteminvamount, itemsoldamount; 	  
			foreach itemstr in GetConfigStringArray( pg_elem, "Item" )
        	//sleepms(100);
    			var mywords:=SplitWords(itemstr);			    	
   			obj_type:=mywords[1];
				itempspawnamount:=CInt(mywords[2]);
				
				//get amount in merchants inventory
				iteminvamount:=0;
				foreach invitem in EnumerateItemsInContainer(inventory)
					if (Hex(invitem.objtype) == Hex(obj_type))
						iteminvamount:=iteminvamount+invitem.amount;	
					endif
				endforeach
				
				itemsoldamount:=itempspawnamount-iteminvamount;
				//Broadcast("obj_type " + obj_type + " itemsoldamount " + itemsoldamount + " itempspawnamount " + itempspawnamount + " iteminvamount " + iteminvamount);
				if (itemsoldamount>0)
					//Broadcast("wrote " + obj_type + " " + itemsoldamount);
					SetEconomyAmount(Hex(obj_type), itemsoldamount, 1);  
				endif
				
			endforeach
 		  
	endforeach
endfunction

function SetEconomyAmount(objtype, amount, bought:=1) //if player bought then 1, if sold 0
//records amount of an objtype in circulation...
//decreases if items bought from merchant (merchant sells)
//increases if items sold to merchant (merchant buys)
	objtype := Hex(objtype);
	if (IsInExemptObjTypes(objtype))
		return 0;
	endif
	var df := GetMerchantEconomyFile();
	var elem := df.FindElement(objtype);
	if (!elem || elem == error)
		elem := df.CreateElement(objtype);
	endif
	var amountboughtsold := CInt(elem.GetProp("boughtSold"));
	if (!amountboughtsold || amountboughtsold==error)
		amountboughtsold:=0;
	endif
	if (bought)
		if (amountboughtsold<-99000)
			//Broadcast("too low");
			UnloadDataFile(FILEPATH);
			return; //dont worry about recording if difference extreme.. will stabilise costs
		endif
		amountboughtsold:=amountboughtsold-amount;
	else
		if (amountboughtsold>99000)
			//Broadcast("too high");
			UnloadDataFile(FILEPATH);
			return; //dont worry about recording if difference extreme.. will stabilise costs
		endif
		
		amountboughtsold:=amountboughtsold+amount;
	endif
	elem.SetProp("boughtSold", CInt(amountboughtsold));
	//Broadcast("written " + objtype + " " + amount);
	UnloadDataFile(FILEPATH);
endfunction

function GetEconomyValueMod(objtype)
//returns multiplier to use on base item sale price in relation to amount in circulation...
	objtype:=Hex(objtype);
	if (IsInExemptObjTypes(objtype))
		return 1;
	endif

	var df := GetMerchantEconomyFile();
	var elem := df.FindElement(objtype);
	if (!elem || elem == error)
		return 1;
	endif
	var amountboughtsold := CInt(elem.GetProp("boughtSold"));
	if (!amountboughtsold || amountboughtsold==error)
		return 1;
	endif
	var temp;
	
	if (amountboughtsold>0) //POSITIVE means oversupply so price drops
		//want this to be between 1 and .1
		//
		//max is 99000
		temp:= 1-(0.1*(amountboughtsold/11000)); 
		if (temp<0.1)
			temp:=0.1;
		endif


	else //NEGATIVE means undersupply so price increases
		amountboughtsold:=abs(amountboughtsold);
		//want this to be between 10 and 1
		//max is -99000
		temp:=1+(amountboughtsold/11000); 
		if (temp>10)
			temp:=10;
		endif		
	endif	
	UnloadDataFile(FILEPATH);
	return temp;
endfunction