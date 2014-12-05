(ns prep.skill_desc
	(:use prep.gen :reload)
	(:use clojure.java.io))
;
;
(def skill-desc 
[{:skill "tower_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 695
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "witch_of_the_lake_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "widow_of_krulah_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "red_dragon_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "magma_golem_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "beholder_of_doom_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "raven_of_krul_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "crypt_cow_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "footmen_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 350
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "archer_of_yssath_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 450
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "woodsman_of_vale_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 450
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "gnome_of_the_woodlands_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 450
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "skeleton_mage_attack"
  :skill-can-cast? :attack
  :skill-cursor :attack
  :skill-shortcut :attack
  :skill-range :attack
  :skill-material :attack
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 450
  :skill-cooldown :attack
  :skill-mana-cost :attack
  :skill-cast-one-by-one false
  :skill-sound :attack
  :skill-look-for-unit :attack
  :skill-look-for-enemy :attack
  :skill-look-for-target :attack
  :skill-look-for-air true
  :skill-look-for-ground true
  :skill-direct-cast :attack
  :skill-can-stop :attack
  :skill-continue :attack
  :skill-is-blocking :attack
  :skill-is-available :attack
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info :attack
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "TRIGGER_ATTACK\nreturn true;\n"}
 {:skill "starshot"
  :skill-can-cast? "\"\""
  :skill-cursor "cursor_spell"
  :skill-shortcut "'W'"
  :skill-range 999
  :skill-material "starshot_mat"
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 345
  :skill-cooldown 100
  :skill-mana-cost 4
  :skill-cast-one-by-one true
  :skill-sound ""
  :skill-look-for-unit false
  :skill-look-for-enemy false
  :skill-look-for-target false
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast false
  :skill-can-stop false
  :skill-continue false
  :skill-is-blocking true
  :skill-is-available true
  :skill-is-passive false
  :skill-is-unique true
  :skill-max-level 1
  :skill-every-level 1
  :skill-info ""
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "#include \"starshot.cpp\"\n"}
 {:skill "moonshot"
  :skill-can-cast? "\"\""
  :skill-cursor "cursor_spell"
  :skill-shortcut "'E'"
  :skill-range 999
  :skill-material "moonshot_mat"
  :skill-cast-speed 1
  :skill-anim-name "Attack"
  :skill-anim-time 1
  :skill-cooldown 1
  :skill-mana-cost 2
  :skill-cast-one-by-one true
  :skill-sound ""
  :skill-look-for-unit false
  :skill-look-for-enemy false
  :skill-look-for-target false
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast false
  :skill-can-stop false
  :skill-continue false
  :skill-is-blocking true
  :skill-is-available true
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info ""
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "#include \"moonshot.cpp\"\n"}
 {:skill "multishot"
  :skill-can-cast? "\"\""
  :skill-cursor "cursor_spell"
  :skill-shortcut "'E'"
  :skill-range 999
  :skill-material "multishot_mat"
  :skill-cast-speed :attack
  :skill-anim-name "Attack"
  :skill-anim-time 345
  :skill-cooldown 10
  :skill-mana-cost 2
  :skill-cast-one-by-one true
  :skill-sound ""
  :skill-look-for-unit false
  :skill-look-for-enemy false
  :skill-look-for-target false
  :skill-look-for-air false
  :skill-look-for-ground true
  :skill-direct-cast false
  :skill-can-stop false
  :skill-continue false
  :skill-is-blocking true
  :skill-is-available true
  :skill-is-passive false
  :skill-is-unique false
  :skill-max-level 6
  :skill-every-level 1
  :skill-info ""
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "#include \"multishot.cpp\"\n"}
 {:skill "invoke_skeleton_mages"
  :skill-can-cast? "\"\""
  :skill-cursor "cursor_spell"
  :skill-shortcut "'W'"
  :skill-range 7
  :skill-material "skeleton_mages_mat"
  :skill-cast-speed 0
  :skill-anim-name "Attack"
  :skill-anim-time 695
  :skill-cooldown 10
  :skill-mana-cost 10
  :skill-cast-one-by-one true
  :skill-sound ""
  :skill-look-for-unit false
  :skill-look-for-enemy false
  :skill-look-for-target false
  :skill-look-for-air false
  :skill-look-for-ground false
  :skill-direct-cast true
  :skill-can-stop false
  :skill-continue false
  :skill-is-blocking true
  :skill-is-available true
  :skill-is-passive false
  :skill-is-unique true
  :skill-max-level 3
  :skill-every-level 1
  :skill-info ""
  :skill-invoke-type (unit-type "footmen")
  :skill-invoke-count 0
  :skill-code "INVOKE(skeleton_mage)\n"}
  ])
;
;
(defmacro per-skill [skill-att & body]
	(let [skills (into [] (map (fn [x] (x skill-att)) skill-desc))]
	  `(str
	    	"switch(skill_type) {\n"
	      (apply
	        str
	        (map
	          (fn [~'i ~'y ~'x]
	            (str "case (TR_skill_type_t)" ~'i ": {\n"
	              ~@body
	              "} break;\n"))
	          (range (count ~skills))
	          ~skills
	          ~skill-desc))
	      "}\n")))
;
;
(defmacro per-skill-att [att attack return? is-string?]
  `(per-skill 
      ~att
      ~(when return? "return ")
      (cond 
        (= :attack ~'y)
          (str (~(if is-string? pr-str identity) ~attack) ";\n")
        :else
          (str (~(if is-string? pr-str identity) ~'y) ";\n"))))
;
;
(defn gen-get-skill-can-stop []
  (per-skill-att :skill-can-stop true true false))
;
;
(defn gen-get-skill-continue []
  (per-skill-att :skill-continue true true false))
;
;
(defn gen-get-skill-is-blocking []
  (per-skill-att :skill-is-blocking true true false))
;
;
(defn gen-get-skill-is-available []
  (per-skill-att :skill-is-available true true false))
;
;
(defn gen-skill-look-for-air []
  (per-skill-att :skill-look-for-air true true false))
;
;
(defn gen-skill-look-for-ground []
  (per-skill-att :skill-look-for-ground true true false))
;
;
(defn gen-skill-cast-one-by-one []
  (per-skill-att :skill-cast-one-by-one false true false))
;
;
(defn gen-get-skill-direct-cast []
  (per-skill-att :skill-direct-cast false true false))
;
;
(defn gen-get-skill-is-passive []
  (per-skill-att :skill-is-passive false true false))
;
;
(defn gen-get-skill-is-unique []
  (per-skill-att :skill-is-unique false true false))
;
;
(defn gen-get-skill-name []
  (per-skill-att :skill "Attack" true true))
;
;
(defn gen-get-skill-anim-name []
  (per-skill-att :skill-anim-name "Attack" true true))
;
;
(defn gen-get-skill-anim-time []
  (per-skill-att :skill-anim-time 0 true false))
;
;
(defn gen-get-skill-range []
  (per-skill-att :skill-range "ATTACK_RANGE(un)" true false))
;
;
(defn gen-get-skill-shortcut []
  (per-skill-att :skill-shortcut "'Q'" true false))
;
;
(defn gen-get-skill-info []
  (per-skill-att :skill-info "Basic Attack. Target the *ground* to attack an area. Target a unit to attack it specifically." true true))
;
;
(defn gen-get-skill-cast-speed []
  (per-skill-att :skill-cast-speed "ATTACK_SPEED(un)" true false))
;
;
(defn gen-get-skill-invoke-type []
  (per-skill-att :skill-invoke-type (unit-type "footmen") true false))
;
;
(defn gen-get-skill-invoke-count []
  (per-skill-att :skill-invoke-count 0 true false))
;
;
(defn gen-get-skill-cooldown []
  (per-skill-att :skill-cooldown (if (= (x :skill-cast-speed) :attack) "ATTACK_SPEED(un)" (x :skill-cast-speed)) true false))
;
;
(defn gen-get-skill-mana-cost []
  (per-skill-att :skill-mana-cost "0" true false))
;
;
(defn gen-can-cast-skill []
  (per-skill-att :skill-can-cast? "\"\"" true false))
;
;
(defn gen-get-cursor []
  (per-skill-att :skill-cursor "cursor_attack" true true))
;
;
(defn gen-get-skill-material []
  (per-skill-att :skill-material "attack_skill_mat" true true))
;
;
(defn gen-skill-looks-for-enemy []
  (per-skill-att :skill-look-for-enemy true true false))
;
;
(defn gen-skill-looks-for-target []
  (per-skill-att :skill-look-for-target false true false))
;
;
(defn gen-skill-looks-for-unit []
  (per-skill-att :skill-look-for-unit true true false))
;
;
(defn gen-get-skill-max-level []
  (per-skill-att :skill-max-level 0 true false))
;
;
(defn gen-get-skill-every-level []
  (per-skill-att :skill-every-level 0 true false))
;
;
(defn gen-on-skill-sound []
  (per-skill-att :skill-sound (str "PLAY_ATTACK_HERE_SOUND(attack_here_footmen)") false false))
;
;
(defn gen-skill-type [x]
  (str "TR_skill_type_" (x :skill) ",\n"))
;
;
(defn gen-skill-cast []
  (per-skill-att :skill-code "" false false))
;
;
(defn gen-skill []
	(save-cpp (gen-can-cast-skill) "gen-can_cast-skill")
  (save-cpp (gen-get-cursor) "gen-get-cursor")
	(save-cpp (gen-get-skill-material) "gen-get-skill-material")
  (save-cpp (gen-get-skill-cast-speed) "gen-get-skill-cast-speed")
  (save-cpp (gen-get-skill-invoke-type) "gen-get-skill-invoke-type")
  (save-cpp (gen-get-skill-invoke-count) "gen-get-skill-invoke-count")
  (save-cpp (gen-get-skill-cooldown) "gen-get-skill-cooldown")
  (save-cpp (gen-get-skill-mana-cost) "gen-get-skill-mana-cost")
  (save-cpp (gen-get-skill-direct-cast) "gen-get-skill-direct-cast")
  (save-cpp (gen-get-skill-is-passive) "gen-get-skill-is-passive")
  (save-cpp (gen-get-skill-is-unique) "gen-get-skill-is-unique")
  (save-cpp (gen-get-skill-can-stop) "gen-get-skill-can-stop")
  (save-cpp (gen-get-skill-continue) "gen-get-skill-continue")
  (save-cpp (gen-get-skill-is-blocking) "gen-get-skill-is-blocking")
  (save-cpp (gen-get-skill-is-available) "gen-get-skill-is-available")
  (save-cpp (gen-skill-look-for-air) "gen-skill-look-for-air")
  (save-cpp (gen-skill-look-for-ground) "gen-skill-look-for-ground")
  (save-cpp (gen-skill-cast-one-by-one) "gen-skill-cast-one-by-one")
  (save-cpp (gen-get-skill-name) "gen-get-skill-name")
  (save-cpp (gen-get-skill-anim-name) "gen-get-skill-anim-name")
  (save-cpp (gen-get-skill-anim-time) "gen-get-skill-anim-time")
  (save-cpp (gen-get-skill-range) "gen-get-skill-range")
  (save-cpp (gen-get-skill-shortcut) "gen-get-skill-shortcut")
  (save-cpp (gen-get-skill-info) "gen-get-skill-info")
  (save-cpp (gen-on-skill-sound) "gen-on-skill-sound")
  (save-cpp (gen-skill-looks-for-enemy) "gen-skill-looks-for-enemy")
  (save-cpp (gen-skill-looks-for-target) "gen-skill-looks-for-target")
  (save-cpp (gen-skill-looks-for-unit) "gen-skill-looks-for-unit")
  (save-cpp (gen-get-skill-max-level) "gen-get-skill-max-level")
  (save-cpp (gen-get-skill-every-level) "gen-get-skill-every-level")
  (save-cpp (gen-skill-cast) "gen-cast-skill")
  (save-cpp (gen-on-desc skill-desc gen-skill-type) "gen-skill-type")
  (println "Skills generated."))