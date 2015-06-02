/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "scenario.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "oldstructs.h"

cScenario::~cScenario() {
	destroy_terrain();
}

void cScenario::destroy_terrain() {
	// Nuke towns
	if(!towns.empty()) {
		for(size_t i = 0; i < towns.size(); i++) {
			if(towns[i] != nullptr) delete towns[i];
			towns[i] = nullptr;
		}
	}
	if(!outdoors.empty()){
		for(size_t i = 0; i < outdoors.width(); i++) {
			for(size_t j = 0; j < outdoors.height(); j++) {
				if(outdoors[i][j] != nullptr) delete outdoors[i][j];
				outdoors[i][j] = nullptr;
			}
		}
	}
}

cScenario& cScenario::operator=(cScenario&& other) {
	// If self-assignment, do nothing.
	if(this == &other) return *this;
	// First, free any held pointers.
	destroy_terrain();
	// Resize the outdoors to ensure the assigned outdoors fits
	outdoors.resize(other.outdoors.width(), other.outdoors.height());
	// Then forward to the default assignment operator.
	// Use const_cast to ensure the right overload is selected.
	return *this = const_cast<const cScenario&>(other);
}

cScenario::cScenario(bool init_strings) {
	short i;
	std::string temp_str;
	
	format.ver[0] = 1;
	format.min_run_ver = 2;
	format.prog_make_ver[0] = 2;
	init_spec = -1;
	default_ground = 2;
	where_start.x = 24;
	where_start.y = 24;
	out_start = where_start;
	bg_out = 10;
	bg_fight = 4;
	bg_town = 13;
	bg_dungeon = 9;
	for(i = 0; i < town_mods.size(); i++) {
		town_mods[i].spec = -1;
	}
	for(i = 0; i < 3; i++) {
		store_item_towns[i] = -1;
	}
	for(i = 0; i < special_items.size(); i++) {
		special_items[i].special = -1;
	}
	for(i = 0; i < scenario_timers.size(); i++) {
		scenario_timers[i].node = -1;
	}
	for(i = 0; i < scen_items.size(); i++) {
		scen_items[i] = cItem();
	}
	if(!init_strings) return;
	for(i = 0; i < 270; i++) {
		temp_str = get_str("scen-default",i + 1);
		if(i == 0) scen_name = temp_str;
		else if(i == 1 || i == 2)
			who_wrote[i-1] = temp_str;
		else if(i == 3)
			contact_info = temp_str;
		else if(i >= 4 && i < 10)
			intro_strs[i-4] = temp_str;
		else if(i >= 10 && i < 60)
			journal_strs[i-10] = temp_str;
		else if(i >= 60 && i < 160) {
			if(i % 2 == 0) special_items[(i-60)/2].name = temp_str;
			else special_items[(i-60)/2].descr = temp_str;
		} else if(i >= 260) continue; // These were never ever used, for some reason.
		else spec_strs[i-160] = temp_str;
	}
}

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

void cScenario::append(legacy::scenario_data_type& old){
	is_legacy = true;
	int i;
	difficulty = old.difficulty;
	intro_pic = old.intro_pic;
	default_ground = old.default_ground * 2;
	intro_mess_pic = old.intro_pic;
	where_start.x = old.where_start.x;
	where_start.y = old.where_start.y;
	out_sec_start.x = old.out_sec_start.x;
	out_sec_start.y = old.out_sec_start.y;
	out_start.x = old.out_start.x;
	out_start.y = old.out_start.y;
	which_town_start = old.which_town_start;
	for(i = 0; i < 10; i++) {
		town_mods[i].spec = old.town_to_add_to[i];
		town_mods[i].x = old.flag_to_add_to_town[i][0];
		town_mods[i].y = old.flag_to_add_to_town[i][1];
		storage_shortcuts[i] = old.storage_shortcuts[i];
	}
	// TODO: Combine store_item_rects and store_item_towns into a structure
	for(i = 0; i < 3; i++) {
		store_item_rects[i].top = old.store_item_rects[i].top;
		store_item_rects[i].left = old.store_item_rects[i].left;
		store_item_rects[i].bottom = old.store_item_rects[i].bottom;
		store_item_rects[i].right = old.store_item_rects[i].right;
		store_item_towns[i] = old.store_item_towns[i];
	}
	for(i = 0; i < 50; i++) {
		special_items[i].flags = old.special_items[i];
		special_items[i].special = old.special_item_special[i];
	}
	rating = old.rating;
	// TODO: Is this used anywhere?
	uses_custom_graphics = old.uses_custom_graphics;
	for(i = 0; i < 30; i++) {
		boats[i].append(old.scen_boats[i]);
		horses[i].append(old.scen_horses[i]);
	}
	scen_specials.resize(256);
	for(i = 0; i < 256; i++){
		ter_types[i].i = i;
		ter_types[i].append(old.ter_types[i]);
		scen_monsters[i].append(old.scen_monsters[i]);
		scen_specials[i].append(old.scen_specials[i]);
	}
	for(i = 0; i < 20; i++) {
		scenario_timers[i].time = old.scenario_timer_times[i];
		scenario_timers[i].node = old.scenario_timer_specs[i];
	}
	last_out_edited.x = old.last_out_edited.x;
	last_out_edited.y = old.last_out_edited.y;
	last_town_edited = old.last_town_edited;
	adjust_diff = true;
}

cScenario::cItemStorage& cScenario::cItemStorage::operator = (legacy::item_storage_shortcut_type& old){
	ter_type = old.ter_type;
	for(int i = 0; i < 10; i++) item_num[i] = old.item_num[i];
	for(int i = 0; i < 10; i++) item_odds[i] = old.item_odds[i];
	property = old.property;
	return *this;
}

void cScenario::append(legacy::scen_item_data_type& old){
	short i;
	for(i = 0; i < 400; i++)
		scen_items[i].append(old.scen_items[i]);
	for(i = 0; i < 256; i++) {
		scen_monsters[i].m_name = old.monst_names[i];
		if(scen_monsters[i].m_type == eRace::UNDEAD && scen_monsters[i].m_name.find("Skeleton") != std::string::npos)
			scen_monsters[i].m_type = eRace::SKELETAL;
		if(scen_monsters[i].m_type == eRace::HUMANOID && scen_monsters[i].m_name.find("Goblin") != std::string::npos)
			scen_monsters[i].m_type = eRace::GOBLIN;
	}
	for(i = 0; i < 256; i++)
		ter_types[i].name = old.ter_names[i];
	// Some default shops - the five magic shops and the healing shop.
	cShop magic_shop('junk');
	for(i = 0; i < 5; i++)
		shops.push_back(magic_shop);
	shops.push_back(cShop('heal'));
}

static std::string format_version(const unsigned char(& ver)[3]) {
	std::ostringstream fmt;
	fmt << int(ver[0]) << '.' << int(ver[1]) << '.' << int(ver[2]);
	return fmt.str();
}

std::string cScenario::format_ed_version() {
	return format_version(format.prog_make_ver);
}

std::string cScenario::format_scen_version() {
	return format_version(format.ver);
}

ter_num_t cScenario::get_ground_from_ter(ter_num_t ter){
	return get_ter_from_ground(ter_types[ter].ground_type);
}

ter_num_t cScenario::get_ter_from_ground(unsigned short ground){
	for(int i = 0; i < ter_types.size(); i++)
		if(ter_types[i].ground_type == ground)
			return i;
	return 0;
}

ter_num_t cScenario::get_trim_terrain(unsigned short ground, unsigned short trim_g, eTrimType trim) {
	for(int i = 0; i < ter_types.size(); i++) {
		if(ter_types[i].ground_type != ground)
			continue;
		if(ter_types[i].trim_ter != trim_g)
			continue;
		if(ter_types[i].trim_type != trim)
			continue;
		return i;
	}
	return 90;
}
