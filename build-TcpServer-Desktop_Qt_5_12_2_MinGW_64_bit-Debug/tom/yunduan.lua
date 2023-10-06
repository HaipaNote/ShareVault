local pubtopic = "attributes"
wifi.setmode(wifi.STATION)
station_cfg = {}
station_cfg.ssid = "chen"
station_cfg.pwd = "12345678"
station_cfg.auto = false
wifi.sta.config(station_cfg)

uart.setup(0, 115200, 8, uart.PARITY_NONE, uart.STOPBITS_1, 0)
gpio.mode(4,gpio.OUTPUT,gpio.PULLUP)

m = mqtt.Client("fdfdfdf", 120, "z0y4o4rbw1ju8ol3", "IqVBcl9ang")
m:on("offline", function(client) print ("MQTT offline!") end)
flag=0
--pwm
local pin = 1  
local speed=100
pwm.setup(pin, 50, 0)
pwm.start(pin)
--pwm
function MQTTpub(b)
    local data = {}
    data.U=b
    local ok, json = pcall(sjson.encode, data)
    if ok then
        m:publish(pubtopic, json, 1, 0, function(client) print("Data published successfully!") end)
    else
        print("JSON encoding failed!")
    end
end

function publishDHTData()
    local status, temp, humi, temp_dec, humi_dec = dht.read11(2)
    if status == dht.OK then
        --MQTTpub(temp, humi)
        print(string.format('{"temperature": %.2f, "humidity": %.2f}', temp, humi))
    else
        print("DHT read error!")
    end
end
--ADC
if adc.force_init_mode(adc.INIT_ADC) then
    node.restart()
    return
  end
  
  local function readVoltage()
    --print(adc.read(0))
    --print(3.3*adc.read(0)/1024)
    local a=3.3*adc.read(0)/1024
    b=tonumber(string.format("%.2f", a))
    print(b.."v")
    MQTTpub(b)
    if flag==0 then
    if a>2.7 then
        pwm.setduty(pin, 110)
    else
        pwm.setduty(pin, 75)
    end
    elseif flag==1 then
        pwm.setduty(pin, 110)
    end  
  end
  local timer = tmr.create()
  timer:register(4000, tmr.ALARM_AUTO, function()
    readVoltage()
  end)
--tmr.create():alarm(2000, tmr.ALARM_AUTO, readVoltage)
--ADC

wifi.sta.connect()
local mytimer = tmr.create()
mytimer:register(1000, tmr.ALARM_AUTO, function()
    if wifi.sta.getip() == nil then
        print("connecting to WiFi...")
    else
        print("WiFi connected, IP is " .. wifi.sta.getip())
        m:connect("bj-2-mqtt.iot-api.com", 1883, false, 
            function(client)
                print("connected to MQTT")
                mytimer:stop()
                m:subscribe("attributes/push",0,function(cnn)
                    print("subsrible success")
                    m:on("message",MQTTSUB)
                    timer:start()
                 end)
                --tmr.create():alarm(4000, tmr.ALARM_AUTO, publishDHTData)
            end,
            function(client, reason)
                print("Connection to MQTT failed reason: " .. reason)
            end)
    end
end)
mytimer:start()

function MQTTSUB(client,topic,data)
print("dd"..data)
    if data=='{"state":false}' then
        gpio.write(4,gpio.HIGH)
        flag=0
    elseif data=='{"state":true}' then
        gpio.write(4,gpio.LOW)
        flag=1
    elseif data=='{"red_state":true}' then
        pwm.setduty(pin, 110)
    elseif data=='{"red_state":false}' then
        pwm.setduty(pin,75)
    end
end

