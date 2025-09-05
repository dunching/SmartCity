命令&前端交互接口

ReplyCameraTransform 

SwitchViewArea 
	Param.1:DefaultGameplayTags.ini->Interaction.Area

SwitchMode 
	Param.1:DefaultGameplayTags.ini->Interaction.Mode

ElevatorMoveToFloor
	Param.1:DefaultGameplayTags.ini->Elevator 
	Param.2:2
	
WeatherSystemCommand
	Param.1:12

AdjustWeather
	Param.1:DefaultGameplayTags.ini->Weather 


程序启动参数
-PixelStreamingURL="ws://127.0.0.1:8888"		像素流地址
			
-AllowPixelStreamingCommands 					

-log 											

-RenderOffScreen

像素流版本：5.4（5.5、5.6网页有错误，具体原因未知）