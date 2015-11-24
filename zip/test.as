package {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	import flash.net.URLLoaderDataFormat;

	import cmodule.z.CLibInit;
	public class test extends Sprite
	{
		private var text:TextField = new TextField();
		public function test()
		{
			//SwfLoader.loadData("/home/libiao/SDL2apk/jni/src/zip/z.swc",loaded,URLLoaderDataFormat.BINARY);
			SwfLoader.loadData("./z.swc",loaded,URLLoaderDataFormat.BINARY);
		}
		private function loaded(e:Event=null):void
		{
			if(e.type == Event.COMPLETE)
			{
				var loader:CLibInit = new CLibInit;
				var lib:Object = loader.init();
				//var bytes:ByteArray= lib.zipList(ByteArray(e.target.data),"notes");
				//var bytes:ByteArray= lib.zipList(ByteArray(e.target.data),"");
				var bytes:ByteArray = lib.zipList(ByteArray(e.target.data),"catalog.xml");
				text.text = String(bytes);
				trace(text.text);
				addChild(text);
				text.width = stage.stageWidth;
				text.height = stage.stageHeight;
				text.border = true;
				text.wordWrap = true;
			}else{
				addChild(new logs);
				logs.adds(e);
			}
		}
	}
}
