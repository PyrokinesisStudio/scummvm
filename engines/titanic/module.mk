MODULE := engines/titanic

MODULE_OBJS := \
	debugger.o \
	detection.o \
	events.o \
	game_location.o \
	game_manager.o \
	game_state.o \
	game_view.o \
	input_handler.o \
	input_translator.o \
	main_game_window.o \
	titanic.o \
	carry/auditory_centre.o \
	carry/arm.o \
	carry/bowl_ear.o \
	carry/brain.o \
	carry/bridge_piece.o \
	carry/carry.o \
	carry/carry_parrot.o \
	carry/central_core.o \
	carry/chicken.o \
	carry/crushed_tv.o \
	carry/ear.o \
	carry/eye.o \
	carry/feathers.o \
	carry/fruit.o \
	carry/glass.o \
	carry/hammer.o \
	carry/head_piece.o \
	carry/hose.o \
	carry/hose_end.o \
	carry/key.o \
	carry/liftbot_head.o \
	carry/long_stick.o \
	carry/magazine.o \
	carry/maitred_left_arm.o \
	carry/maitred_right_arm.o \
	carry/mouth.o \
	carry/napkin.o \
	carry/nose.o \
	carry/note.o \
	carry/parcel.o \
	carry/perch.o \
	carry/phonograph_cylinder.o \
	carry/phonograph_ear.o \
	carry/photograph.o \
	carry/plug_in.o \
	carry/speech_centre.o \
	carry/sweets.o \
	carry/test_carry.o \
	carry/vision_centre.o \
	core/background.o \
	core/click_responder.o \
	core/dont_save_file_item.o \
	core/drop_target.o \
	core/file_item.o \
	core/game_object.o \
	core/game_object_desc_item.o \
	core/link_item.o \
	core/list.o \
	core/mail_man.o \
	core/message_target.o \
	core/movie_clip.o \
	core/multi_drop_target.o \
	core/named_item.o \
	core/node_item.o \
	core/project_item.o \
	core/resource_key.o \
	core/room_item.o \
	core/saveable_object.o \
	core/static_image.o \
	core/turn_on_object.o \
	core/turn_on_play_sound.o \
	core/turn_on_turn_off.o \
	core/tree_item.o \
	core/view_item.o \
	game/announce.o \
	game/annoy_barbot.o \
	game/arb_background.o \
	game/arboretum_gate.o \
	game/auto_animate.o \
	game/bilge_succubus.o \
	game/bomb.o \
	game/bar_menu.o \
	game/bar_menu_button.o \
	game/bar_bell.o \
	game/belbot_get_light.o \
	game/bottom_of_well_monitor.o \
	game/bomb.o \
	game/bowl_unlocker.o \
	game/brain_slot.o \
	game/bridge_door.o \
	game/bridge_view.o \
	game/broken_pell_base.o \
	game/broken_pellerator.o \
	game/broken_pellerator_froz.o \
	game/call_pellerator.o \
	game/cage.o \
	game/captains_wheel.o \
	game/cdrom.o \
	game/cdrom_computer.o \
	game/cdrom_tray.o \
	game/cell_point_button.o \
	game/chev_code.o \
	game/chev_panel.o \
	game/chicken_cooler.o \
	game/chicken_dispensor.o \
	game/close_broken_pel.o \
	game/code_wheel.o \
	game/cookie.o \
	game/computer.o \
	game/computer_screen.o \
	game/credits.o \
	game/credits_button.o \
	game/dead_area.o \
	game/desk_click_responder.o \
	game/doorbot_elevator_handler.o \
	game/doorbot_home_handler.o \
	game/ear_sweet_bowl.o \
	game/eject_phonograph_button.o \
	game/elevator_action_area.o \
	game/emma_control.o \
	game/empty_nut_bowl.o \
	game/end_credit_text.o \
	game/end_credits.o \
	game/end_explode_ship.o \
	game/end_game_credits.o \
	game/end_sequence_control.o \
	game/hammer_dispensor.o \
	game/hammer_dispensor_button.o \
	game/fan.o \
	game/fan_control.o \
	game/fan_decrease.o \
	game/fan_increase.o \
	game/fan_noises.o \
	game/floor_indicator.o \
	game/games_console.o \
	game/get_lift_eye2.o \
	game/glass_smasher.o \
	game/hammer_clip.o \
	game/head_slot.o \
	game/head_smash_event.o \
	game/head_smash_lever.o \
	game/head_spinner.o \
	game/idle_summoner.o \
	game/leave_sec_class_state.o \
	game/lemon_dispensor.o \
	game/light.o \
	game/light_switch.o \
	game/little_lift_button.o \
	game/long_stick_dispenser.o \
	game/missiveomat.o \
	game/missiveomat_button.o \
	game/movie_tester.o \
	game/music_console_button.o \
	game/music_room_phonograph.o \
	game/music_room_stop_phonograph_button.o \
	game/music_system_lock.o \
	game/musical_instrument.o \
	game/nav_helmet.o \
	game/navigation_computer.o \
	game/no_nut_bowl.o \
	game/nose_holder.o \
	game/null_port_hole.o \
	game/nut_replacer.o \
	game/pet_disabler.o \
	game/phonograph.o \
	game/phonograph_lid.o \
	game/play_music_button.o \
	game/play_on_act.o \
	game/port_hole.o \
	game/record_phonograph_button.o \
	game/replacement_ear.o \
	game/reserved_table.o \
	game/restaurant_cylinder_holder.o \
	game/restaurant_phonograph.o \
	game/sauce_dispensor.o \
	game/search_point.o \
	game/season_background.o \
	game/season_barrel.o \
	game/seasonal_adjustment.o \
	game/service_elevator_window.o \
	game/ship_setting.o \
	game/ship_setting_button.o \
	game/show_cell_points.o \
	game/speech_dispensor.o \
	game/splash_animation.o \
	game/starling_puret.o \
	game/start_action.o \
	game/stop_phonograph_button.o \
	game/sub_glass.o \
	game/sub_wrapper.o \
	game/sweet_bowl.o \
	game/television.o \
	game/third_class_canal.o \
	game/tow_parrot_nav.o \
	game/throw_tv_down_well.o \
	game/titania_still_control.o \
	game/up_lighter.o \
	game/useless_lever.o \
	game/volume_control.o \
	game/wheel_button.o \
	game/wheel_hotspot.o \
	game/wheel_spin.o \
	game/wheel_spin_horn.o \
	game/gondolier/gondolier_base.o \
	game/gondolier/gondolier_chest.o \
	game/gondolier/gondolier_face.o \
	game/gondolier/gondolier_mixer.o \
	game/gondolier/gondolier_slider.o \
	game/maitred/maitred_arm_holder.o \
	game/maitred/maitred_body.o \
	game/maitred/maitred_legs.o \
	game/maitred/maitred_prod_receptor.o \
	game/parrot/parrot_lobby_controller.o \
	game/parrot/parrot_lobby_link_updater.o \
	game/parrot/parrot_lobby_object.o \
	game/parrot/parrot_lobby_view_object.o \
	game/parrot/parrot_loser.o \
	game/parrot/parrot_nut_bowl_actor.o \
	game/parrot/parrot_nut_eater.o \
	game/parrot/parrot_perch_holder.o \
	game/parrot/parrot_succubus.o \
	game/parrot/parrot_trigger.o \
	game/parrot/player_meets_parrot.o \
	game/pet/pet.o \
	game/pet/pet_class1.o \
	game/pet/pet_class2.o \
	game/pet/pet_class3.o \
	game/pet/pet_lift.o \
	game/pet/pet_monitor.o \
	game/pet/pet_pellerator.o \
	game/pet/pet_position.o \
	game/pet/pet_sentinal.o \
	game/pet/pet_sounds.o \
	game/pet/pet_transition.o \
	game/pet/pet_transport.o \
	game/pickup/pick_up.o \
	game/pickup/pick_up_bar_glass.o \
	game/pickup/pick_up_hose.o \
	game/pickup/pick_up_lemon.o \
	game/pickup/pick_up_speech_centre.o \
	game/pickup/pick_up_vis_centre.o \
	game/placeholder/bar_shelf_vis_centre.o \
	game/placeholder/place_holder.o \
	game/placeholder/lemon_on_bar.o \
	game/placeholder/tv_on_bar.o \
	game/transport/gondolier.o \
	game/transport/lift.o \
	game/transport/lift_indicator.o \
	game/transport/pellerator.o \
	game/transport/service_elevator.o \
	game/transport/transport.o \
	game/sgt/armchair.o \
	game/sgt/basin.o \
	game/sgt/bedfoot.o \
	game/sgt/bedhead.o \
	game/sgt/chest_of_drawers.o \
	game/sgt/desk.o \
	game/sgt/deskchair.o \
	game/sgt/drawer.o \
	game/sgt/sgt_doors.o \
	game/sgt/sgt_nav.o \
	game/sgt/sgt_navigation.o \
	game/sgt/sgt_restaurant_doors.o \
	game/sgt/sgt_state_control.o \
	game/sgt/sgt_state_room.o \
	game/sgt/sgt_tv.o \
	game/sgt/sgt_upper_doors_sound.o \
	game/sgt/toilet.o \
	game/sgt/vase.o \
	game/sgt/washstand.o \
	gfx/act_button.o \
	gfx/changes_season_button.o \
	gfx/chev_left_off.o \
	gfx/chev_left_on.o \
	gfx/chev_right_off.o \
	gfx/chev_right_on.o \
	gfx/chev_send_rec_switch.o \
	gfx/chev_switch.o \
	gfx/edit_control.o \
	gfx/elevator_button.o \
	gfx/get_from_succ.o \
	gfx/helmet_on_off.o \
	gfx/home_photo.o \
	gfx/icon_nav_action.o \
	gfx/icon_nav_butt.o \
	gfx/icon_nav_down.o \
	gfx/icon_nav_image.o \
	gfx/icon_nav_left.o \
	gfx/icon_nav_receive.o \
	gfx/icon_nav_right.o \
	gfx/icon_nav_send.o \
	gfx/icon_nav_up.o \
	gfx/keybrd_butt.o \
	gfx/move_object_button.o \
	gfx/music_control.o \
	gfx/send_to_succ.o \
	gfx/sgt_selector.o \
	gfx/slider_button.o \
	gfx/small_chev_left_off.o \
	gfx/small_chev_left_on.o \
	gfx/small_chev_right_off.o \
	gfx/small_chev_right_on.o \
	gfx/status_change_button.o \
	gfx/st_button.o \
	gfx/text_down.o \
	gfx/text_skrew.o \
	gfx/text_up.o \
	gfx/toggle_button.o \
	gfx/toggle_switch.o \
	messages/auto_sound_event.o \
	messages/bilge_auto_sound_event.o \
	messages/bilge_dispensor_event.o \
	messages/door_auto_sound_event.o \
	messages/messages.o \
	messages/service_elevator_door.o \
	moves/enter_bomb_room.o \
	moves/enter_bridge.o \
	moves/enter_exit_first_class_state.o \
	moves/enter_exit_mini_lift.o \
	moves/enter_exit_sec_class_mini_lift.o \
	moves/enter_exit_view.o \
	moves/enter_sec_class_state.o \
	moves/exit_arboretum.o \
	moves/exit_bridge.o \
	moves/exit_lift.o \
	moves/exit_pellerator.o \
	moves/exit_state_room.o \
	moves/exit_tiania.o \
	moves/move_player_in_parrot_room.o \
	moves/move_player_to_from.o \
	moves/move_player_to.o \
	moves/multi_move.o \
	moves/pan_from_pel.o \
	moves/restaurant_pan_handler.o \
	moves/restricted_move.o \
	moves/scraliontis_table.o \
	moves/trip_down_canal.o \
	npcs/barbot.o \
	npcs/bellbot.o \
	npcs/callbot.o \
	npcs/character.o \
	npcs/deskbot.o \
	npcs/doorbot.o \
	npcs/liftbot.o \
	npcs/maitre_d.o \
	npcs/mobile.o \
	npcs/parrot.o \
	npcs/robot_controller.o \
	npcs/starlings.o \
	npcs/succubus.o \
	npcs/summon_bots.o \
	npcs/titania.o \
	npcs/true_talk_npc.o \
	pet_control/pet_control.o \
	pet_control/pet_conversations.o \
	pet_control/pet_element.o \
	pet_control/pet_frame.o \
	pet_control/pet_gfx_element.o \
	pet_control/pet_inventory.o \
	pet_control/pet_inventory_glyphs.o \
	pet_control/pet_message.o \
	pet_control/pet_nav_helmet.o \
	pet_control/pet_real_life.o \
	pet_control/pet_remote.o \
	pet_control/pet_remote_glyphs.o \
	pet_control/pet_rooms.o \
	pet_control/pet_rooms_glyphs.o \
	pet_control/pet_section.o \
	pet_control/pet_drag_chev.o \
	pet_control/pet_graphic2.o \
	pet_control/pet_graphic.o \
	pet_control/pet_glyphs.o \
	pet_control/pet_leaf.o \
	pet_control/pet_load.o \
	pet_control/pet_load_save.o \
	pet_control/pet_mode_off.o \
	pet_control/pet_mode_on.o \
	pet_control/pet_mode_panel.o \
	pet_control/pet_pannel1.o \
	pet_control/pet_pannel2.o \
	pet_control/pet_pannel3.o \
	pet_control/pet_quit.o \
	pet_control/pet_save.o \
	pet_control/pet_slider.o \
	pet_control/pet_sound.o \
	pet_control/pet_text.o \
	sound/auto_music_player.o \
	sound/auto_music_player_base.o \
	sound/auto_sound_player.o \
	sound/auto_sound_player_adsr.o \
	sound/background_sound_maker.o \
	sound/bird_song.o \
	sound/dome_from_top_of_well.o \
	sound/enter_view_toggles_other_music.o \
	sound/gondolier_song.o \
	sound/music_room.o \
	sound/music_player.o \
	sound/node_auto_sound_player.o \
	sound/restricted_auto_music_player.o \
	sound/room_auto_sound_player.o \
	sound/room_trigger_auto_music_player.o \
	sound/season_noises.o \
	sound/seasonal_music_player.o \
	sound/sound.o \
	sound/sound_manager.o \
	sound/titania_speech.o \
	sound/trigger_auto_music_player.o \
	sound/view_auto_sound_player.o \
	sound/view_toggles_other_music.o \
	sound/water_lapping_sounds.o \
	sound/wave_file.o \
	star_control/star_control.o \
	star_control/star_control_sub1.o \
	star_control/star_control_sub2.o \
	star_control/star_control_sub3.o \
	star_control/star_control_sub4.o \
	star_control/star_control_sub5.o \
	star_control/star_control_sub6.o \
	star_control/star_control_sub7.o \
	star_control/star_control_sub8.o \
	star_control/star_control_sub9.o \
	star_control/star_control_sub10.o \
	star_control/star_control_sub11.o \
	star_control/star_control_sub12.o \
	star_control/star_control_sub13.o \
	star_control/star_control_sub14.o \
	star_control/star_control_sub15.o \
	support/direct_draw.o \
	support/direct_draw_surface.o \
	support/exe_resources.o \
	support/files_manager.o \
	support/font.o \
	support/image.o \
	support/image_decoders.o \
	support/mouse_cursor.o \
	support/movie.o \
	support/movie_event.o \
	support/movie_range_info.o \
	support/proximity.o \
	support/rect.o \
	support/screen_manager.o \
	support/simple_file.o \
	support/string.o \
	support/text_cursor.o \
	support/time_event_info.o \
	support/video_surface.o \
	true_talk/barbot_script.o \
	true_talk/bellbot_script.o \
	true_talk/deskbot_script.o \
	true_talk/dialogue_file.o \
	true_talk/doorbot_script.o \
	true_talk/liftbot_script.o \
	true_talk/maitred_script.o \
	true_talk/parrot_script.o \
	true_talk/succubus_script.o \
	true_talk/title_engine.o \
	true_talk/script_handler.o \
	true_talk/true_talk_manager.o \
	true_talk/tt_action.o \
	true_talk/tt_adj.o \
	true_talk/tt_hist.o \
	true_talk/tt_major_word.o \
	true_talk/tt_npc_script.o \
	true_talk/tt_parser.o \
	true_talk/tt_picture.o \
	true_talk/tt_pronoun.o \
	true_talk/tt_room_script.o \
	true_talk/tt_script_base.o \
	true_talk/tt_scripts.o \
	true_talk/tt_sentence.o \
	true_talk/tt_string.o \
	true_talk/tt_string_node.o \
	true_talk/tt_synonym.o \
	true_talk/tt_talker.o \
	true_talk/tt_title_script.o \
	true_talk/tt_vocab.o \
	true_talk/tt_word.o

# This module can be built as a plugin
ifeq ($(ENABLE_TITANIC), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
