use os;

function GHIC_CreateIssue(title, body, labels := array{})

	var method := "POST";
	var url := "https://api.github.com/repos/zuluhotelaustralia/zha/issues";
	var oAuth := GetConfigString(FindConfigElem(ReadConfigFile( "::secrets" ), "github"), "github_auth");
	var header := struct{
		"Accept" := "application/vnd.github+json",
		"Authorization" := "Bearer "+oAuth,
		"X-GitHub-Api-Version":="2022-11-28",
		"User-Agent" := "Chrome"
	};
	var LabelsOutput := "", i;
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
  		\"labels\":["+LabelsOutput+"]
	}";

	var req := HTTPRequest(url, method, options := struct{
	            "headers" := header,
	            "data" := data
	            });
	return req;

endfunction