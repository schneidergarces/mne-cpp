int Sample32bit(unsigned char *byteArray) {
	
		int result = (
                 	byteArray[0] << 16 |  
                 	byteArray[1] << 8 |   
                 	byteArray[2]  
               );
  
 		if (result > 8388608) // 2^24/2
            {
                result = result - 16777216; // 2^24
            }
		return result;
		
		}