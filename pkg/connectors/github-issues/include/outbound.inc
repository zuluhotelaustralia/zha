use os;

function GHIC_CreateIssue(title, body, labels := array{})

	var method := "POST";
	var url := "https://api.github.com/repos/zuluhotelaustralia/zha/issues";
	var oAuth := "5ef6a23e77cbc7d1316a65cd6e0fbfee61f3b95d";
	var header := struct{
		"Accept" := "application/vnd.github.VERSION.text+json",
		"Authorization" := "token "+oAuth,
		"User-Agent" := "Chrome"
	};
	var LabelsOutput, i;
	for( i := 1; i <= labels.size(); i++)
		LabelsOutput += "\"";
		LabelsOutput += CStr(labels[i]);
		if( i != labels.size() )
			LabelsOutput += "\",";
		else
			LabelsOutput += "\"";
		endif
	endfor
	var data :=
	  	"{\"title\":\""+title+"\",
  		\"body\": \""+body+"\",
  		\"labels\":[
  			\"bug\"
  		]
	}";


	var req := HTTPRequest(url, method, options := struct{
	            "headers" := header,
	            "data" := data
	            });
	return req;

endfunction