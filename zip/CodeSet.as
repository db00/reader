package
{
	import flash.utils.ByteArray;
	public class CodeSet{
		public static function urlencode(s:String):String {
			/*
			   var PageEncoding:String = WebUtils.PageEncoding;
			   if(PageEncoding.match(/utf-?8/i)){
			   return s;
			   }else if(PageEncoding.length >0){
			   }
			 */
			return String(s);
			/*return encodeURI(s);*/
			/*return encodeURIComponent(s);*/
		}
		public static function urldecode(s:String):String {
			if(s as String){
				try{
					var _s:String = decodeURI(String(s).replace(/[\"\\\/]/g,""));
					return _s;
				}catch(e:Error){logs.adds(e,_s);}
			}
			return String(s);
			/*return decodeURIComponent(s);*/
		}

		public static function iconv(src:String="中文",code:String="GBK"):String
		{
			/*
			   var tmpbytearr:ByteArray = ByteArray(src);
			   tmpbytearr.position = 0;
			   des = tmpbytearr.readMutiByte(tmpbytearr.length, code);
			   trace(des);
			   return des;
			   var des:String = "";
			   var tmpbytearr:ByteArray = new ByteArray();
			   tmpbytearr.writeMultiByte(src, code);//参看注解四
			   tmpbytearr.position = 0;//重要！！！参看注解1
			   for (var j=0; j<tmpbytearr.length; j++)
			   {
			   des += tmpbytearr.readUnsignedByte().toString(16);
			   }
			   trace(des);//d6d0cec4
			   return des;
			 */
			return src;
		}
	}
}
