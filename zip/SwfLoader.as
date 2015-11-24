package 
{
	import flash.display.Bitmap;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequestMethod;
	import flash.net.URLRequest;
	import flash.net.URLVariables;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.system.SecurityDomain;
	import flash.utils.ByteArray;
	/**
	 * 素材加载器
	 * ...
	 * @author db0@qq.com
	 */
	public class SwfLoader 
	{
		private var loadedFunction:Function;
		private var curURLLoader:URLLoader;
		private var curLoader:Loader;
		/**
		 * 
		 * @param	url
		 * @param	loaded
		 */
		public function SwfLoader(url:String=null,loaded:Function=null):void
		{
			loadedFunction = loaded;
			if(url){
				curURLLoader = loadData(url,loadcomplete,URLLoaderDataFormat.BINARY);
			}
		}

		public function sandboxLoadSwf(url:String=null,loaded:Function=null):URLLoader
		{
			loadedFunction = loaded;
			if(url){
				return loadData(url,loadcomplete,URLLoaderDataFormat.BINARY);
			}
			return null;
		}

		private function loadcomplete(e:Event):void 
		{
			if (e && e.type == Event.COMPLETE) {
				curLoader = loadBytes(e.target.data,loadedFunction);
			}else {
				logs.adds("load error",e);
			}
		}


		public static function loadBytes(bytes:ByteArray,loaded:Function):Loader
		{
			if(loaded ==null)return null;
			var loader:Loader = new Loader();
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loaded);
			loader.contentLoaderInfo.addEventListener(SecurityErrorEvent.SECURITY_ERROR, loaded);
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaded);
			var context:LoaderContext = new LoaderContext(false, ApplicationDomain.currentDomain);
			context.allowCodeImport = true;
			loader.loadBytes(bytes,context);
			return loader;
		}

		public static function SwfLoad(url:String,loaded:Function):Loader
		{
			var loader:Loader = new Loader();
			loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loaded);
			loader.contentLoaderInfo.addEventListener(SecurityErrorEvent.SECURITY_ERROR, loaded);
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaded);

			var context:LoaderContext = new LoaderContext(false);
			context.applicationDomain = new ApplicationDomain(ApplicationDomain.currentDomain) ; 
			context.allowCodeImport = true;
			loader.load(new URLRequest(url),context);
			logs.adds("url:",url);
			return loader;
		}

		public static function loadData(url:String,loaded:Function,type:String=null,urlVariables:URLVariables=null):URLLoader
		{
			var loader:URLLoader = new URLLoader();
			var request:URLRequest = new URLRequest(url);
			logs.adds("url:",url);
			if(urlVariables){
				logs.adds(CodeSet.urldecode(String(urlVariables)));
				request.data = urlVariables;
				request.method = URLRequestMethod.POST;
			}
			if(type)loader.dataFormat = type;

			/*if(urlVariables) loader.dataFormat = URLLoaderDataFormat.VARIABLES;*/

			loader.addEventListener(IOErrorEvent.IO_ERROR, loaded);
			loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, loaded);
			loader.addEventListener(Event.COMPLETE, loaded);
			loader.load(request);
			return loader;
		}

		public static function closeLoader(loader:Loader):void
		{
			if(loader){
				if(loader.parent)loader.parent.removeChild(loader);
				try{
					loader.close();
				}catch(e:Error){
					logs.adds(e);
				}
				loader.unloadAndStop();
				loader = null;
			}
		}

		public function close():void
		{
			if(curURLLoader){
				try{
					curURLLoader.close();
					curURLLoader = null;
				}catch(e:Error){
					logs.adds(e);
				}
			}
			if(curLoader){
				if(curLoader.parent)curLoader.parent.removeChild(curLoader);
				try{
					curLoader.close();
				}catch(e:Error){
					logs.adds(e);
				}
				try{
					curLoader.unloadAndStop();
				}catch(e:Error){
					logs.adds(e);
				}
				curLoader = null;
			}
		}


		public static function closeURLLoader(loader:URLLoader):void
		{
			if(loader){
				try{
					loader.close();
					loader = null;
				}catch(e:Error){
					logs.adds(e);
				}
			}
		}


		public static const imgReg:String = "(jpg$)|(png$)|(jpeg$)|(gif$)";
		public static const mp3Reg:String = "mp3$";
		public static const swfReg:String = "swf$";
		public static const txtReg:String = "txt$";

		public static function matchImgs(url:String):Boolean
		{
			if (url)
				return Boolean(url.match(new RegExp(imgReg, "i")));
			return false;
		}

		public static function matchTxts(url:String):Boolean
		{
			if (url)
				return Boolean(url.match(new RegExp(txtReg, "i")));
			return false;
		}

		public static function matchmp3(url:String):Boolean
		{
			if (url)
				return Boolean(url.match(new RegExp(mp3Reg, "i")));
			return false;
		}
		public static function matchSwf(url:String):Boolean
		{
			if (url)
				return Boolean(url.match(new RegExp(swfReg, "i")));
			return false;
		}
		/*
		   public static function sortFun(file1:File,file:File):int{
		   var num1:Number =Number(file.name.replace(/^([\d]+)[^\d].*$/i,"$1"));
		   var num2:Number =Number(file1.name.replace(/^([\d]+)[^\d].*$/i,"$1"));
		   if(file.name.match(/^[\d]+$/))num1 = Number(file.name);
		   if(file1.name.match(/^[\d]+$/))num2 = Number(file1.name);
		   var num :Number = num2 - num1;

		   if(num > 0)return 1;
		   if(num < 0)return -1;

		   num1 = Number(file.name.replace(/^.*[^\d]([\d]+)[^\d]*$/i,"$1"));
		   num2 = Number(file1.name.replace(/^.*[^\d]([\d]+)[^\d]*$/i,"$1"));
		   num = num2 - num1;

		   if(num > 0)return 1;
		   if(num < 0)return -1;
		   return 0;
		   }
		 */
	}
}
