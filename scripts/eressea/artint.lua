local npc = {}

local npcmsg = {
    "Pay me 10000 silver or die!",
    "I shall take your life with my blade!",
    "I dare you to attack me!",
    "If you survive, you shall be rewarded!"
}

local function npc_brain(u)
  local u = get_unit(atoi36("bohu"))
  local r = u.region
for f in factions() do
  local i = math.randomseed(os.time())
  local i = math.random(#npcmsg)
  local msg = message.create("msg_event")
  msg:set_string("string", "A message from " .. tostring(u) .. ": " ..npcmsg[i])
  -- msg:send_region(r)
  msg:send_faction(f)
  u:clear_orders()
  u:add_order("LEARN melee")
end
end

function npc.init()
    local f = get_faction(atoi36("npc")) -- find NPC faction
    local u = get_unit(atoi36("bohu"))
    if not f then
        eressea.log.error("NPC is missing, will re-create")
        local r = get_region(0,0)
        local f = faction.create("npcs@npcs.com", "human", "en")
		f.id = atoi36("npc")
		f.name = "NPC Army"
		f.flags = 50331648 -- npc flag so units don't starve or require orders
		f.options = 3
        if r and f then
            u = unit.create(f, r, 1)
            u.id = atoi36("bohu")
            u.name = "Bounty Hunter"
            u.info = "Suffer my wrath!"
            u.race = "troll"
			u.race_name = "Pirate"
			-- add misc items and skills for testing ... need to update after testing process
            u:add_item("sword", 1)
			u:add_item("plate", 1)
			u:add_item("shield", 1)
			u:add_item("money", 1000)
			u:set_skill("melee", 15)
			u:set_skill("endurance", 10)
			u:set_skill("perception", 50)
			u:set_skill("tactics", 10)
        else
            eressea.log.error("Failure to find or create NPC faction")
        end
    else
        eressea.log.error("Found NPC faction")
    end
    if u then
        npc_brain(u)
    end
    local f = get_faction(atoi36("ii")) -- find monster faction
    if not f then
	eressea.log.error("Monsters are missing, will re-create")
	local r = get_region(0,0)
       	local f = faction.create("eresseahl@gmail.com", "dragon", "en")
	f.id = atoi36("ii")
	f.name = "Monster"
	f.flags = 50331648
	f.options = 3 -- receive reports, but no template
    else
	-- f.locale = "en" -- only use this if defualt language is de
	eressea.log.error("Found Monsters")
    end
end

return npc
