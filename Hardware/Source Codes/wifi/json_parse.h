String get_id(String json) {
   String key = "\"id\"";
   int key_len = 4;
   int json_len = json.length();
   for(int i = 0; i < json_len; ++i) {
      String temp = json.substring(i, i + key_len);
      if(temp.compareTo(key) == 0) {
        int start_index = i + key_len + 1;
        int end_index;
        
        for(int j = start_index + 1; j < json_len; ++j) {
            if(json.charAt(j) == ',') {
                end_index = j - 1;
                break;
            }
        }
        
        return json.substring(start_index, end_index + 1);
      }
   }
   return String("");
}


String get_write_api(String json) { //input is the value of "api_keys"
   String key = "\"write_flag\":true";
   int key_len = key.length();
   int json_len = json.length();
   for(int i = 0; i < json_len; ++i) {
      String temp = json.substring(i, i + key_len);
      if(temp.compareTo(key) == 0) {
        int start_index;
        int end_index = i - 3;
        for(int j = end_index - 1; j > 0; --j) {
            if(json.charAt(j) == '\"') {
                start_index = j + 1;
                break;
            }
        }
        return json.substring(start_index, end_index + 1);
      }
   }  
}

String get_read_api(String json) { //input is the value of "api_keys"
   String key = "\"write_flag\":false";
   int key_len = key.length();
   int json_len = json.length();
   for(int i = 0; i < json_len; ++i) {
      String temp = json.substring(i, i + key_len);
      if(temp.compareTo(key) == 0) {
        int start_index;
        int end_index = i - 3;
        for(int j = end_index - 1; j > 0; --j) {
            if(json.charAt(j) == '\"') {
                start_index = j + 1;
                break;
            }
        }
        return json.substring(start_index, end_index + 1);
      }
   }  
}



