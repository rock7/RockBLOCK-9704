-- THIS FILE SERVES AS A BASIC EXAMPLE FOR HOW TO USE THE ROCKBLOCK 9704 LIBRARY IN LUA.
--
-- CHANGE THE CONFIGURATION AS REQUIRED AT THE TOP OF THE FILE
-- IT WILL:
--    CONNECT TO THE ROCKBLOCK AND PRINT THE DETAILS OF THE DEVICE
--    WAIT FOR SINGAL FOR THE PESCRIBED TIME 
--    WHEN SIGNAL IS FOUND IT WILL ATTEMPT TO SEND A MESSAGE
--
-- THERE ARE MORE EXAMPLES COMMENTED OUT BELOW FOR SENDING TO SPECIFIC TOPICS AND RECEIVING MESSAGES


-----------------------------------------------------------------------------------------------------
-- LOAD THE ROCKBLOCK MODULE
-----------------------------------------------------------------------------------------------------

-- WINDOWS
-- The dll must be placed accesible to this script (same directory is easy)
-- the name of the dll must match the require (rockblock_lua.dll)

-- LINUX
-- The dll must be placed accesible to this script (same directory is easy)

-- MACOS
-- The dylib must be placed accesible to this script (same directory is easy)
-- The name of the dylib must match the require (rockblock_lua.dylib)
-- The Lua cpath may need to be adjusted as below
-- package.cpath = package.cpath .. ";?.dylib"


local rockblock = require("rockblock_lua")

-- Check if the module loaded successfully
if rockblock then
    print("rockblock_lua module loaded successfully!")
else
    print("Failed to load rockblock_lua module!")
    return -- Exit if module didn't load
end

-----------------------------------------------------------------------------------------------------
-- SCRIPT CONFIGURATION (CHANGE AS REQUIRED)
-----------------------------------------------------------------------------------------------------

local port_name = "COM8"              -- local port with the rockBlock 9704 connected to

local max_duration_seconds = 3 * 60   -- 3 minutes (this is to wait for signal, and attempting to receive messages)
local loop_delay_seconds = 1          -- Wait 1 second between attempts

local max_sending_runtime = 240       -- maximum time to try and send a message
local max_receiving_runtime = 240     -- maximum time to try and receive a message


-----------------------------------------------------------------------------------------------------
-- HELPER FUNCTIONS
-----------------------------------------------------------------------------------------------------
-- check OS (based on whether it uses backslash or forward)
local function getOs()
    return package.config:sub(1,1) == "\\" and "win" or "unix"
end

-- hacky sleep function for windows
local function windowsSleep(n)
  if n > 0 then os.execute("ping -n " .. tonumber(n+1) .. " localhost > NUL") end
end

-- hacky sleep function for linux
local function linuxSleep(n)
  os.execute("sleep " .. tonumber(n))
end

-- sleep
local function sleep(n)
  if getOs() == "win" then
    windowsSleep(n)
  else
    linuxSleep(n)
  end
end

local function getCurrentDateTimeString()
    return os.date("%m/%d/%Y, %H:%M:%S")
end

-- generate some random bytes to create a messages of a specified size
local function generateBytes(target)
    local date_time_str = getCurrentDateTimeString()

    local base_str = "Hello, from Lua at " .. date_time_str

    local base_len = #base_str

    local remaining_size = target - base_len

    if remaining_size > 0 then
        local padding_chars = {}

        for i = 1, remaining_size do
            local random_byte = math.random(0, 255)
            padding_chars[i] = string.char(random_byte)
        end
        
        local padding_str = table.concat(padding_chars)

        return base_str .. padding_str
    end

    return base_str
end


-----------------------------------------------------------------------------------------------------
-- ROCKBLOCK RUNNING CODE
-----------------------------------------------------------------------------------------------------

-- first call to rockblock 9704!
local success = rockblock.rbBegin(port_name)

if success then
  print("\nrbBegin called successfully for port: " .. port_name .. ". Result: true")
  
  -- board information
  local hardware_version = rockblock.rbGetHwVersion()
  print("Hardware version: " .. tostring(hardware_version))
  
  local firmware_version = rockblock.rbGetFirmwareVersion()
  print("Firmware version: " .. tostring(firmware_version))
  
  local board_temp = rockblock.rbGetBoardTemp()
  print("Board temperature: " .. tostring(board_temp) .. " C")
  
  
  -- Identifying Information
  local serial_number = rockblock.rbGetSerialNumber()
  print("Serial number: " .. tostring(serial_number))
  
  local imei = rockblock.rbGetImei()
  print("IMEI: " .. tostring(imei))
  
  local iccid = rockblock.rbGetIccid()
  print("ICCID: " .. tostring(iccid))
  
  
  -- Check present simcard
  local card_present = rockblock.rbGetCardPresent()
  local sim_connected = rockblock.rbGetSimConnected()
  if card_present and sim_connected then
      print("Sim card valid")
  else
      print("Sim card not valid (Card Present: " .. tostring(card_present) .. ", Sim Connected: " .. tostring(sim_connected) .. ")")
  end
  
  
  -- signal Acquisition Loop 
  print("\nStarting signal acquisition loop (max " .. max_duration_seconds .. " seconds...")
  
  local start_time = os.time()
  local signal_found = false
  local final_signal_value = nil
  
  while os.difftime(os.time(), start_time) < max_duration_seconds do
    local current_elapsed_time = os.difftime(os.time(), start_time)
    
    local signal_value = rockblock.rbGetSignal()
    
    if type(signal_value) == "number" and signal_value > 0 then
      signal_found = true
      final_signal_value = signal_value
      print("Signal found! Value: " .. final_signal_value)
      break
    else
      print("Signal not yet found. Calling rbGetSignal...")
      sleep(loop_delay_seconds)
    end
  end
  
  
  -- After the loop, report the final status and send messages
  if signal_found then
    print("Signal acquired successfully. Final Signal Strength: " .. final_signal_value)
    
    
    -- send a test message
    print("\nAttempting to send test message...")
    local message1 = generateBytes(100);
    local sentMessage1 = rockblock.rbSendMessage(message1, #message1, max_sending_runtime)
    
    if sentMessage1 then
      print("rbSendMessage: Message sent successfully!")
    else
      print("rbSendMessage: Failed to send message.")
    end


    -- -- send a test message to a defined topic
    -- print("\nAttempting to send topic test message...")
    -- local message2 = generateBytes(100);
    -- local sentMessage2 = rockblock.rbSendMessageCloudloop(rockblock.PINK_TOPIC, message2, #message2, max_sending_runtime)
    -- 
    -- if sentMessage2 then
    --   print("rbSendMessageCloudloop: Topic message sent successfully!")
    -- else
    --   print("rbSendMessageCloudloop: Failed to send topic message.")
    -- end
    
    -- -- send a test message to a specific topic
    -- print("\nAttempting to send specific topic test message...")
    -- local message3 = generateBytes(100);
    -- local sentMessage3 = rockblock.rbSendMessageAny(317, message3, #message3, max_sending_runtime)
    -- 
    -- if sentMessage2 then
    --   print("rbSendMessageAny: Any message sent successfully!")
    -- else
    --   print("rbSendMessageAny: Failed to send topic message.")
    -- end
    
    
    -- -- receive message
    -- print("\nAttempting to receive messages...")
    -- local start_receive_time = os.time()
    -- while os.difftime(os.time(), start_receive_time) < max_duration_seconds do
    --   local current_elapsed_time = os.difftime(os.time(), start_receive_time)
    --   
    --   local message_length, message_content = rockblock.rbReceiveMessage()
    --   
    --   if message_length > 0 and message_content ~= nil then
    --     print("Message Received: \"" .. message_content .. "\"")
    --     print("Message length: " .. message_length)
    --     break
    --   else
    --     print("Messages not yet found. Calling rbReceiveMessage...")
    --     sleep(loop_delay_seconds)
    --   end
    --   
    -- end
    
    --  -- receive message from specific topic
    -- print("\nAttempting to receive messages from topic...")
    -- local start_receive_time = os.time()
    -- while os.difftime(os.time(), start_receive_time) < max_duration_seconds do
    --   local current_elapsed_time = os.difftime(os.time(), start_receive_time)
    --   
    --   local message_length2, message_content2 = rockblock.rbReceiveMessageWithTopic(316)
    --   
    --   if message_length2 > 0 and message_content2 ~= nil then
    --     print("Message Received: \"" .. message_content2 .. "\"")
    --     print("Message length: " .. message_length2)
    --     break
    --   else
    --     print("Messages not yet found. Calling rbReceiveMessageWithTopic...")
    --     sleep(loop_delay_seconds)
    --   end
    --   
    -- end
  
  
  else
    print("Time limit reached. Signal was not found (value > 0) within the duration.")
  end
    

else
  print("rbBegin failed for port: " .. port_name .. ". Result: false")
end


print("RockBlock Lua script finished")
