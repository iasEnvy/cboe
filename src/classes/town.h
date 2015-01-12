/*
 *  town.h
 *  BoE
 *
 *  Created by Celtic Minsrel on 22/04/09.
 *
 */

#ifndef BOE_DATA_TOWN_H
#define BOE_DATA_TOWN_H

#include <vector>
#include <iosfwd>
#include <array>
#include "simpletypes.h"
#include "location.h"
#include "special.h"
#include "monster.h"
#include "talking.h"

namespace legacy {
	struct town_record_type;
	struct big_tr_type;
	struct ave_tr_type;
	struct tiny_tr_type;
	struct creature_start_type;
	struct wandering_type;
	struct preset_item_type;
	struct preset_field_type;
};

enum eLighting {
	LIGHT_NORMAL = 0,
	LIGHT_DARK = 1,
	LIGHT_DRAINS = 2,
	LIGHT_NONE = 3,
};

class cScenario;

class cTown { // formerly town_record_type
protected:
	cScenario& scenario;
public:
//	class cCreature { // formerly creature_start_type
//	public:
//		unsigned short number;
//		unsigned char start_attitude;
//		location start_loc;
//		unsigned char mobile;
//		unsigned char time_flag;
//		unsigned char extra1,extra2;
//		short spec1, spec2;
//		char spec_enc_code,time_code;
//		short monster_time,personality;
//		short special_on_kill,facial_pic;
//		
//		cCreature& operator = (legacy::creature_start_type old);
//	};
	class cWandering { // formerly wandering_type
	public:
		m_num_t monst[4];
		
		bool isNull();
		void append(legacy::wandering_type old);
	};
	class cItem { // formerly preset_item_type
	public:
		location loc;
		short code,ability;
		unsigned char charges;
		bool always_there, property, contained;
		
		void append(legacy::preset_item_type old);
	};
	class cField { // formerly preset_field_type
	public:
		location loc;
		eFieldType type;
		
		void append(legacy::preset_field_type old);
		cField() : type(FIELD_DISPEL) {}
	};
	short town_chop_time,town_chop_key;
	cWandering wandering[4];
	location wandering_locs[4];
	location special_locs[50];
	unsigned short spec_id[50];
	location sign_locs[15];
	eLighting lighting_type;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	rectangle in_town_rect;
	cItem preset_items[64];
	short max_num_monst;
	std::vector<cField> preset_fields;
	short spec_on_entry,spec_on_entry_if_dead;
	short spec_on_hostile;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	std::array<cSpecial,100> specials;
	bool strong_barriers : 1;
	bool defy_mapping : 1;
	bool defy_scrying : 1;
	char reserved_bits : 5;
	short hostile_fry_party; // number of a special to be called when the town goes hostile TODO: Not sure about this yet though
	short difficulty;
	std::string town_name;
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::array<std::string,16> rect_names;
	std::array<std::string,3> comment;
	std::array<std::string,100> spec_strs;
	std::array<std::string,20> sign_strs;
	cSpeech talking;
	
	virtual ~cTown(){}
	virtual void append(legacy::big_tr_type& old, int town_num);
	virtual void append(legacy::ave_tr_type& old, int town_num);
	virtual void append(legacy::tiny_tr_type& old, int town_num);
	virtual ter_num_t& terrain(size_t x, size_t y) = 0;
	virtual rectangle& room_rect(size_t i) = 0;
	virtual cCreature& creatures(size_t i) = 0;
	virtual unsigned char& lighting(size_t i, size_t r) = 0;
	virtual short max_dim() const = 0;
	virtual short max_monst() const = 0;
	virtual short max_items() const = 0;
	void init_start();
	void set_up_lights();
	short light_obscurity(short x,short y); // Obscurity function used for calculating lighting
	
	explicit cTown(cScenario& scenario, bool init_strings = false);
	void append(legacy::town_record_type& old);
	virtual void writeTerrainTo(std::ostream& file) = 0;
	virtual void readTerrainFrom(std::istream& file) = 0;
};

#endif
