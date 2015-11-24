package
{
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.filters.GlowFilter;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class logs extends Sprite
	{
		public static var txt:TextField = new TextField();
		
		public function logs()
		{
			addChild(txt);
			adds("12.04@author :db0@qq.com,");
			if(txt.stage)init(null);
			else txt.addEventListener(Event.ADDED_TO_STAGE, init);
		}
		
		public static function init(e:Event):void
		{
			txt.removeEventListener(Event.ADDED_TO_STAGE, init);
			txt.filters = [new GlowFilter(0xffffff)];
			/*txt.mouseEnabled = false;*/
			/*txt.selectable = false;*/
			/*txt.visible = false;*/
			txt.multiline = true;
			//txt.textColor = 0xffffff;
			//txt.background = true;
			txt.defaultTextFormat = new TextFormat(null, 15);
			txt.wordWrap = true;
			txt.scrollV = txt.maxScrollV;
			txt.stage.addEventListener(KeyboardEvent.KEY_UP, show);
		}
		
		static public function show(e:KeyboardEvent):void
		{
			
			switch(String.fromCharCode(e.charCode)) {
				case "H":
					txt.visible = !(txt.visible);
					break;
				case "R":
					txt.mouseEnabled = true;
					txt.selectable = true;
					break;
			}
		}
		
		private static var line:int = 0;
		
		public static function adds(str:*, ... other):void
		{
			if (txt.stage) {
				txt.stage.addChild(txt);
				txt.width = txt.stage.stageWidth/2;
				txt.height = txt.stage.stageHeight/2;
			}
			
			//txt.appendText("\n" +(++line) + ":" +str);
			//txt.appendText("\n" + String(str));
			txt.appendText("\n" +String(str).replace(/[\r\s\n]/gm," "));
			for each (var item:*in other)
			{
				txt.appendText("," + String(item).replace(/[\r\s\n]/gm," "));
			}
			
			trace(str, other);
			txt.scrollV = txt.maxScrollV;
		}
	}

}
