命令&前端交互接口

ReplyCameraTransform 

{
	"CMD":"SwitchViewArea" ，
	"GUID":“”，
	“AreaTag”:“Interaction.Area.ExternalWall”\“Interaction.Area.Floor.F1”、“Interaction.Area.Floor.B1”
}

{
	"CMD":"AdjustHour" ，
	"GUID":“”，
	“Hour”:0~24
}

{
	"CMD":"AdjustWeather" ，
	"GUID":“”，
	“Weather”:Clear_Skies、Cloudy、Foggy、Overcast、Partly_Cloudy、Rain、Rain_Light、Rain_Thunderstorm、Sand_Dust_Calm、Sand_Dust_Storm、Snow、Snow_Blizzard、Snow_Light
}

{
	"CMD":"SwitchMode" ，
	"GUID":“”，
	“Weather”:
		"Interaction.Mode.DeviceManagger"、
		"Interaction.Mode.DeviceManagger.SunShadow"、
		"Interaction.Mode.DeviceManagger.PWR.Lighting"、
		"Interaction.Mode.DeviceManagger.PWR.HVAC"、
		"Interaction.Mode.DeviceManagger.ELV.Radar"、
		"Interaction.Mode.EnvironmentalPerception"、
		"Interaction.Mode.EnergyManagement"、
		"Interaction.Mode.EmergencySystem"、
}

{
	"CMD":"SceneItemOption" ，
	"GUID":“”，
	"Wall":0~100
	"Pillar":0~100
	"Furniture":true
}

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
-PixelStreamingURL="ws://127.0.0.1:8888" -RenderOffScreen -log 	

-PixelStreamingURL="ws://127.0.0.1:8888"		像素流地址
			
-AllowPixelStreamingCommands 					

-log 											

-RenderOffScreen

像素流版本：5.4（5.5、5.6网页有错误，具体原因未知）