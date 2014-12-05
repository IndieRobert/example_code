(ns prep.unit_desc
  (:use prep.gen :reload)
  (:use clojure.java.io))

(defn train-unit [unit_type]
  (str "gi->unit_dic[TR_unit_type_"unit_type"].gold_price <= cam_mpl->gold ? \"\" : \"Not enought gold.\""))

(defmacro attack_type [x]
  `~(str "TR_attack_type_" x))
(defmacro armor_type [x]
  `~(str "TR_armor_type_" x))
(defmacro skill_type [x]
  (if (= x -1)
    `~(str "(TR_skill_type_t)-1")  
    `~(str "TR_skill_type_" x)))
(defmacro ball-mesh-cinetic-type [x]
  `~(str "TR_ball_mesh_cinetic_type_" x))
(defn height [x]
  (int (* x 512)))

(def unit-desc 
[{:name "halfling_of_the_hills"
  :mesh ["halfling_of_the_hills" "halfling_of_the_hills2"]
  :select-scale 1
  :model-scale 1
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  65
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 0.5)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 0.5)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh1"
  :ball-mesh-size 0.5
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 5
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "crypt_cow"
  :mesh ["crypt_cow" "crypt_cow2"]
  :select-scale 1
  :model-scale 1.25
  :bar-height 0.75
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 50
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh5"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "blue_fireball_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "crypt_cow_attack")]
  :default-kill (skill_type "crypt_cow_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "vorguul"
  :mesh ["vorguul" "vorguul2"]
  :select-scale 1
  :model-scale 1
  :bar-height 0.45
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  40
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                0
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 20
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  25
  :aggro-range 7
  :attack-range 6
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh6"
  :ball-mesh-size 1.5
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   2
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "fireball_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills []
  :default-kill "(TR_skill_type_t)-1"
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying true
  :has-magic-vision false
  :is-hero false}
 {:name "beholder_of_doom"
  :mesh ["beholder_of_doom" "beholder_of_doom2"]
  :select-scale 1
  :model-scale 1
  :bar-height 1
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  40
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 3)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live 0
  :hp                           (hp 5)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 50
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   2
  :visibility  17
  :aggro-range 7
  :attack-range 6
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh6"
  :ball-mesh-size 1.5
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   2
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "fireball_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "beholder_of_doom_attack")]
  :default-kill (skill_type "beholder_of_doom_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying true
  :has-magic-vision false
  :is-hero false}
 {:name "common_skeleton_warrior"
  :mesh ["common_skeleton_warrior" "common_skeleton_warrior2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 4)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh2"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 5
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "cursed_chest"
  :mesh ["cursed_chest" "cursed_chest2"]
  :select-scale 1
  :model-scale 1.25
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 2)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live 0
  :hp                           (hp 5)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "heavy")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh2"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "guard_of_the_misty_keep"
  :mesh ["guard_of_the_misty_keep" "guard_of_the_misty_keep2"]
  :select-scale 1
  :model-scale 1.25
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 3)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 3)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "heavy")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh1"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "footmen"
  :mesh ["footmen" "footmen2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh1"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "warrior_of_mushof"
  :mesh ["warrior_of_mushof" "warrior_of_mushof2"]
  :select-scale 1
  :model-scale 1.25
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  45
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 2)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 6)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "heavy")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh3"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "raven_of_krul"
  :mesh ["raven_of_krul" "raven_of_krul2"]
  :select-scale 1
  :model-scale 1.15
  :bar-height 0.66
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  75
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 1)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 3
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "rock"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 3
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   1
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "rock_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  1
  :ball-explosion-additive?  false
  :skills [(skill_type "raven_of_krul_attack")]
  :default-kill (skill_type "raven_of_krul_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying true
  :has-magic-vision false
  :is-hero false}
 {:name "witch_of_the_lake"
  :mesh ["witch_of_the_lake" "witch_of_the_lake2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.66
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  50
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 4)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 3
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "poison"
  :ball-mesh-size 1.5
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   1
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "poison_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "witch_of_the_lake_attack")]
  :default-kill (skill_type "witch_of_the_lake_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying true
  :has-magic-vision false
  :is-hero false}
 {:name "dark_bat_of_ida"
  :mesh ["dark_bat_of_ida" "dark_bat_of_ida2"]
  :select-scale 1
  :model-scale 1.15
  :bar-height 0.66
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  60
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 2)
  :attack-damage-per-level 0
  :attack-type (attack_type "chaos")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 2
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "poison"
  :ball-mesh-size 1.5
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   1
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "poison_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "widow_of_krulah_attack")]
  :default-kill (skill_type "widow_of_krulah_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying true
  :has-magic-vision false
  :is-hero false}
 {:name "widow_of_krulah"
  :mesh ["widow_of_krulah" "widow_of_krulah2"]
  :select-scale 1
  :model-scale 0.75
  :bar-height 1
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  60
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 5)
  :attack-damage-per-level 0
  :attack-type (attack_type "chaos")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 80
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   2
  :visibility  17
  :aggro-range 7
  :attack-range 2
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "poison"
  :ball-mesh-size 2
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "poison_explosion"
  :ball-explosion-size  2
  :ball-explosion-frame  1.85
  :ball-explosion-additive?  false
  :skills [(skill_type "widow_of_krulah_attack")]
  :default-kill (skill_type "widow_of_krulah_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "red_dragon_of_the_titraknus"
  :mesh ["red_dragon_of_the_titraknus" "red_dragon_of_the_titraknus2"]
  :select-scale 1
  :model-scale 1.25
  :bar-height 1
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  35
  :attack-speed  3000
  :attack-speed-per-level 325
  :attack-damage                (damage 12)
  :attack-damage-per-level 0
  :attack-type (attack_type "chaos")
  :time-to-live 0
  :hp                           (hp 20)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 80
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "fortified")
  :size   2
  :visibility  17
  :aggro-range 7
  :attack-range 7
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "big_fireball"
  :ball-mesh-size 3
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "fireball_explosion"
  :ball-explosion-size  4
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "red_dragon_attack")]
  :default-kill (skill_type "red_dragon_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "magma_golem_of_lacdragrath"
  :mesh ["magma_golem_of_lacdragrath" "magma_golem_of_lacdragrath2"]
  :select-scale 1
  :model-scale 0.85
  :bar-height 1
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  35
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 6)
  :attack-damage-per-level 0
  :attack-type (attack_type "chaos")
  :time-to-live 0
  :hp                           (hp 10)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 80
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "fortified")
  :size   2
  :visibility  17
  :aggro-range 7
  :attack-range 5
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "big_meteor"
  :ball-mesh-size 2
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "fireball_explosion"
  :ball-explosion-size  3
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "magma_golem_attack")]
  :default-kill (skill_type "magma_golem_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "savage_of_ridgar"
  :mesh ["savage_of_ridgar" "savage_of_ridgar2"]
  :select-scale 1
  :model-scale 1.18
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  55
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 3)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 4)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh4"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "disciple_of_waja"
  :mesh ["disciple_of_waja" "disciple_of_waja2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  65
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 4)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                           (hp 1)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh2"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "thief_of_tavratt"
  :mesh ["thief_of_tavratt" "thief_of_tavratt2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  55
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 2)
  :attack-damage-per-level 0
  :attack-type (attack_type "piercing")
  :time-to-live 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh2"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "mask_of_vaaror"
  :mesh ["mask_of_vaaror" "mask_of_vaaror2"]
  :select-scale 1
  :model-scale 1.0
  :bar-height 0.6
  :gold  25
  :xp   10
  :build-time  5000
  :move-speed  70
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage                (damage 4)
  :attack-damage-per-level 0
  :attack-type (attack_type "piercing")
  :time-to-live 0
  :hp                           (hp 1)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 1
  :anim-speed 1.33
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "woosh2"
  :ball-mesh-size 0.66
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 6
  :ball-offset [0 0 0.5]
  :ball-height (height 0.6)
  :ball-size   3
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "slash"
  :ball-explosion-size  0.7
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "footmen_attack")]
  :default-kill (skill_type "footmen_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim true
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "woodsman_of_vale"
  :mesh ["woodsman_of_vale" "woodsman_of_vale2"]
  :select-scale 1.0
  :model-scale 1
  :bar-height 0.6
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  60
  :attack-speed  2000
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "normal")
  :time-to-live                 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 30
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "medium")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 4
  :anim-speed 1
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "rock"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0.15 0 0.5]
  :ball-height (height 0.25)
  :ball-size   1.3
  :ball-en-cloche? true
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "rock_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  1
  :ball-explosion-additive?  false
  :skills [(skill_type "woodsman_of_vale_attack")]
  :default-kill (skill_type "woodsman_of_vale_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "archer_of_thelmeril"
  :mesh ["archer_of_thelmeril" "archer_of_thelmeril2"]
  :select-scale 1.0
  :model-scale 1
  :bar-height 0.6
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  60
  :attack-speed  2000
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "piercing")
  :time-to-live                 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 30
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 5
  :anim-speed 1
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "arrow"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0.15 0 0.5]
  :ball-height (height 0.25)
  :ball-size   1.3
  :ball-en-cloche? true
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "arrowexplosion"
  :ball-explosion-size  1
  :ball-explosion-frame  1
  :ball-explosion-additive?  false
  :skills [(skill_type "archer_of_yssath_attack")]
  :default-kill (skill_type "archer_of_yssath_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "archer_of_yssath"
  :mesh ["archer_of_yssath" "archer_of_yssath2"]
  :select-scale 1.0
  :model-scale 1
  :bar-height 0.6
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  70
  :attack-speed  2000
  :attack-speed-per-level 325
  :attack-damage                (damage 1)
  :attack-damage-per-level 0
  :attack-type (attack_type "piercing")
  :time-to-live                 0
  :hp                           (hp 1)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 30
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 7
  :anim-speed 1
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "arrow"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0.15 0 0.5]
  :ball-height (height 0.25)
  :ball-size   1.3
  :ball-en-cloche? true
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "arrowexplosion"
  :ball-explosion-size  1
  :ball-explosion-frame  1
  :ball-explosion-additive?  false
  :skills [(skill_type "archer_of_yssath_attack")]
  :default-kill (skill_type "archer_of_yssath_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "moon_gnome"
  :mesh ["gnome_of_the_woodlands" "gnome_of_the_woodlands2"]
  :select-scale 1.0
  :model-scale 1
  :bar-height 0.6
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  65
  :attack-speed  2000
  :attack-speed-per-level 325
  :attack-damage                (damage 3)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live                 0
  :hp                           (hp 1)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 30
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 4
  :anim-speed 1
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "liteball"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0.15 0 0.5]
  :ball-height (height 0.25)
  :ball-size   1.3
  :ball-en-cloche? true
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "liteball_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "gnome_of_the_woodlands_attack") (skill_type "moonshot")]
  :default-kill (skill_type "gnome_of_the_woodlands_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "skeleton_mage"
  :mesh ["skeleton_mage" "skeleton_mage2"]
  :select-scale 1.0
  :model-scale 1
  :bar-height 0.6
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  70
  :attack-speed  2000
  :attack-speed-per-level 325
  :attack-damage                (damage 4)
  :attack-damage-per-level 0
  :attack-type (attack_type "magic")
  :time-to-live                 0
  :hp                           (hp 2)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 30
  :mn-bar-size 30
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size   1
  :visibility  17
  :aggro-range 7
  :attack-range 5
  :anim-speed 1
  :can-be-pushed?   true
  :ball-particle  ""
  :ball-mesh "fireball"
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 2
  :ball-offset [0.15 0 0.5]
  :ball-height (height 0.25)
  :ball-size   1.3
  :ball-en-cloche? true
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "fireball_explosion"
  :ball-explosion-size  1
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "skeleton_mage_attack")]
  :default-kill (skill_type "skeleton_mage_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood true
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "tower"
  :mesh ["tower"]
  :select-scale 1
  :model-scale 1.0
  :bar-height (* 0.6 2)
  :gold  75
  :xp   30
  :build-time  5000
  :move-speed  0
  :attack-speed  1250
  :attack-speed-per-level 325
  :attack-damage            (damage 3)
  :attack-damage-per-level  0
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                       (hp 40)
  :hp-per-level 0
  :hp-regen 0
  :hp-regen-per-level 0
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :hp-bar-size 60
  :mn-bar-size 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "fortified")
  :size   2
  :visibility  17
  :aggro-range 10
  :attack-range             6
  :anim-speed 1.33
  :can-be-pushed?   false
  :ball-particle  ""
  :ball-mesh "big_blue_fireball"
  :ball-mesh-size 2
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 4
  :ball-offset [0 0 0]
  :ball-height (height 0)
  :ball-size   2
  :ball-en-cloche? false
  :ball-additive?   false
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  "blue_fireball_explosion"
  :ball-explosion-size  2
  :ball-explosion-frame  2
  :ball-explosion-additive?  false
  :skills [(skill_type "tower_attack")]
  :default-kill (skill_type "tower_attack")
  :can-be-built? true
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood false
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "inhibitor"
  :mesh ["inhibitor"]
  :select-scale 1
  :model-scale 1
  :bar-height (* 0.6 4)
  :gold    0
  :xp   0
  :build-time      0
  :move-speed   0
  :attack-speed  1000
  :attack-speed-per-level 325
  :attack-damage    0
  :attack-damage-per-level 30
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                 (hp 150)
  :hp-per-level 3
  :hp-regen 350
  :hp-regen-per-level 55
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size  4
  :visibility  20
  :aggro-range 1
  :attack-range 1
  :anim-speed 1.0
  :can-be-pushed?  false
  :ball-particle  "lineStreak"
  :ball-mesh ""
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 10
  :ball-offset [0 0 0]
  :ball-height 0
  :ball-size   4
  :ball-en-cloche? false
  :ball-additive?    true
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  ""
  :ball-explosion-size  1
  :ball-explosion-frame  4
  :ball-explosion-additive?  false
  :hp-bar-size 100
  :mn-bar-size 100
  :skills []
  :default-kill (skill_type -1)
  :can-be-built? false
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood false
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
 {:name "human_cauldron"
  :mesh ["ancient"]
  :select-scale 1
  :model-scale 1
  :bar-height (* 0.6 4)
  :gold    0
  :xp   0
  :build-time      0
  :move-speed   0
  :attack-speed  1000
  :attack-speed-per-level 325
  :attack-damage    0
  :attack-damage-per-level 30
  :attack-type (attack_type "normal")
  :time-to-live 0
  :hp                 (hp 150)
  :hp-per-level 3
  :hp-regen 350
  :hp-regen-per-level 55
  :mn    0
  :mn-regen 0
  :mn-regen-per-level 0
  :mn-per-level 0
  :armor  0
  :armor-per-level 0
  :armor-type (armor_type "light")
  :size  4
  :visibility  20
  :aggro-range 1
  :attack-range 1
  :anim-speed 1.0
  :can-be-pushed?  false
  :ball-particle  "lineStreak"
  :ball-mesh ""
  :ball-mesh-size 1
  :ball-mesh-cinetic-type (ball-mesh-cinetic-type "none")
  :ball-speed 10
  :ball-offset [0 0 0]
  :ball-height 0
  :ball-size   4
  :ball-en-cloche? false
  :ball-additive?    true
  :ball-explosion-sound "gp_gi.basic_hit_sound"
  :ball-explosion-particle  ""
  :ball-explosion-mesh  ""
  :ball-explosion-size  1
  :ball-explosion-frame  4
  :ball-explosion-additive?  false
  :hp-bar-size 100
  :mn-bar-size 100
  :skills []
  :default-kill (skill_type -1)
  :can-be-built? false
  :build-shortcut "K"
  :info "yoooopi bhala didon"
  :unlock-xp 50
  :blend-with-pose false
  :start-ball-with-attack-anim false
  :die-with-blood false
  :y-angle 0
  :y-offset 0
  :flying false
  :has-magic-vision false
  :is-hero false}
  ])

;
;
(def sound-variation_count 3)
;
;
(def action-list ["select" "move" "attack_here" "attack" "dead"])
;
;
(defn check-sounds [x]
  (let [dir "../data/"]
    (doall
      (mapcat
        (fn [i]
          (map
            (fn [action]
              (let [f (str dir action "_" (x :name) i ".wav")]
                (when (not (.exists (as-file f)))
                  (copy-file (str dir action "_empty.wav") f))))
            action-list))
        (range sound-variation_count)))))
;
;
(defmacro get-type []
  `(str "TR_unit_type_" (~'x :name)))
;
;
(defmacro per-type [ & body]
  `(str
      "case " (get-type) ": {\n"
      ~@body
      "} break;\n"))
;
;
(defn gen-get-pskill-material [x]
  (per-type (str "return " (pr-str (x :pskill-material)) ";\n")))
;
;
(defn gen-get-skill-count [x]
  (per-type (str "return " (count (x :skills)) ";\n")))
;
;
(defn gen-get-unit-name [x]
  (per-type
    (str 
      "return " 
      "\""
      (clojure.string/join 
        " " 
        (map 
          clojure.string/capitalize 
          (clojure.string/split 
            (clojure.string/replace (x :name) "_" " ") 
            #" ")))
      "\";\n")))
;
;
(defn gen-on-attack-unit-sound [x]
  (per-type (str "PLAY_ATTACK_SOUND(" (x :name) ")\n")))
;
;
(defn gen-on-move-unit-sound [x]
  (per-type (str "PLAY_MOVE_SOUND(" (x :name) ")\n")))
;
;
(defn gen-on-select-unit-sound [x]
  (per-type (str "PLAY_SELECT_SOUND(" (x :name) ")\n")))
;
;
(defn gen-play-dead-sound [x]
  (per-type (str "PLAY_DEAD_SOUND(" (x :name) ")\n")))
;
;
(defn gen-sound-decl [x]
  (apply
    str
    (map 
      str
      (repeat "*")
      action-list
      (repeat (str "_" (x :name) "[" sound-variation_count "],\n")))))
;
;
(defn gen-sound-ini [x]
  (apply
    str
    (map
      str
      (mapcat 
        (fn [i]
          (map 
            str
            (repeat "gp_gi.")
            action-list
            (repeat (str "_" (x :name) "[" i "] = OPEN_SOUND(\"./data/"))
            action-list
            (repeat (str "_" (x :name) i ".wav\", \"wav\");\n"))))
        (range sound-variation_count)))))
;
;
(defn gen-sound-rel [x]
  (apply
    str
    (mapcat
      (fn [i]
        (map 
          str
          (repeat "ga_sound_release(gp_gi.")
          action-list
          (repeat (str "_" (x :name) "[" i "]);\n"))))
      (range sound-variation_count))))
;
;
(defn gen-unit-def [x]
  (str 
    "gi->unit_dic[" (get-type) "].info = " (pr-str (x :info)) ";\n"
    "gi->unit_dic[" (get-type) "].gold_price = " (x :gold) ";\n"
    "gi->unit_dic[" (get-type) "].xp = " (x :xp) ";\n"
    "gi->unit_dic[" (get-type) "].level = 1;\n"
    "gi->unit_dic[" (get-type) "].build_time = " (x :build-time) ";\n"
    "gi->unit_dic[" (get-type) "].move_speed = " (x :move-speed) ";\n"
    "gi->unit_dic[" (get-type) "].attack_speed = " (x :attack-speed) ";\n"
    "gi->unit_dic[" (get-type) "].attack_speed_per_level = " (x :attack-speed-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].attack_damage = " (x :attack-damage) ";\n"
    "gi->unit_dic[" (get-type) "].attack_damage_per_level = " (x :attack-damage-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].attack_type = " (x :attack-type) ";\n"
    "gi->unit_dic[" (get-type) "].time_to_live = " (x :time-to-live) ";\n"
    "gi->unit_dic[" (get-type) "].max_hp = " (x :hp) ";\n"
    "gi->unit_dic[" (get-type) "].hp_regen = " (x :hp-regen) ";\n"
    "gi->unit_dic[" (get-type) "].hp_regen_per_level = " (x :hp-regen-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].hp_per_level = " (x :hp-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].max_mana = " (x :mn) ";\n"
    "gi->unit_dic[" (get-type) "].mana_regen = " (x :mn-regen) ";\n"
    "gi->unit_dic[" (get-type) "].mana_regen_per_level = " (x :mn-regen-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].mana_per_level = " (x :mn-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].armor = " (x :armor) ";\n"
    "gi->unit_dic[" (get-type) "].armor_per_level = " (x :armor-per-level) ";\n"
    "gi->unit_dic[" (get-type) "].armor_type = " (x :armor-type) ";\n"
    "gi->unit_dic[" (get-type) "].can_be_pushed = " (x :can-be-pushed?) ";\n"
    "gi->unit_dic[" (get-type) "].size = " (x :size) ";\n"
    "gi->unit_dic[" (get-type) "].visibility_range = " (x :visibility) ";\n"
    "gi->unit_dic[" (get-type) "].aggro_range = " (x :aggro-range) ";\n"
    "gi->unit_dic[" (get-type) "].attack_range = " (x :attack-range) ";\n"
    "gi->unit_dic[" (get-type) "].ball_particle = " (pr-str (x :ball-particle)) ";\n"
    "gi->unit_dic[" (get-type) "].ball_mesh = " (pr-str (x :ball-mesh)) ";\n"
    "gi->unit_dic[" (get-type) "].ball_mesh_size = " (x :ball-mesh-size) ";\n"
    "gi->unit_dic[" (get-type) "].ball_mesh_cinetic_type = " (x :ball-mesh-cinetic-type) ";\n"
    "gi->unit_dic[" (get-type) "].ball_speed = " (x :ball-speed) ";\n"
    "gi->unit_dic[" (get-type) "].ball_offset = Ogre::Vector3(" (nth (x :ball-offset) 0) ", " (nth (x :ball-offset) 1) ", " (nth (x :ball-offset) 2) ");\n"
    "gi->unit_dic[" (get-type) "].ball_height = " (x :ball-height) ";\n"
    "gi->unit_dic[" (get-type) "].ball_size = " (x :ball-size) ";\n"
    "gi->unit_dic[" (get-type) "].ball_en_cloche = " (x :ball-en-cloche?) ";\n"
    "gi->unit_dic[" (get-type) "].ball_additive = " (x :ball-additive?) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.unit_type = TR_unit_type_" (x :name) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.explosion_sound = " (x :ball-explosion-sound) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.particle = " (pr-str (x :ball-explosion-particle)) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.mesh_name = " (pr-str (x :ball-explosion-mesh)) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.size = " (x :ball-explosion-size) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.total_frame = " (x :ball-explosion-frame) ";\n"
    "gi->unit_dic[" (get-type) "].explosion_type.additive = " (x :ball-explosion-additive?) ";\n"
    "gi->unit_dic[" (get-type) "].blend_with_pose = " (x :blend-with-pose) ";\n"
    "gi->unit_dic[" (get-type) "].start_ball_with_attack_anim = " (x :start-ball-with-attack-anim) ";\n"
    "gi->unit_dic[" (get-type) "].die_with_blood = " (x :die-with-blood) ";\n"
    "gi->unit_dic[" (get-type) "].y_angle = " (x :y-angle) ";\n"
    "gi->unit_dic[" (get-type) "].y_offset = " (x :y-offset) ";\n"
    "gi->unit_dic[" (get-type) "].flying = " (x :flying) ";\n"
    "gi->unit_dic[" (get-type) "].has_magic_vision = " (x :has-magic-vision) ";\n"
    "gi->unit_dic[" (get-type) "].is_hero = " (x :is-hero) ";\n"
    "gi->unit_dic[" (get-type) "].skill_count = " (count (x :skills)) ";\n"
    (apply 
      str
      (map
        (fn [y i]
          (str "gi->unit_dic[" (get-type) "].skills[" i "] = " y ";\n"))
        (x :skills)
        (range (count (x :skills)))))
    (apply 
      str
      (map
        (fn [y i]
          (str "gi->unit_dic[" (get-type) "].mesh[" i "] = " (pr-str y) ";\n"))
        (x :mesh)
        (range (count (x :mesh)))))
    "gi->unit_dic[" (get-type) "].mesh_count = " (count (x :mesh)) ";\n"
    "gi->unit_dic[" (get-type) "].select_scale = " (pr-str (x :select-scale)) ";\n"
    "gi->unit_dic[" (get-type) "].model_scale = " (pr-str (x :model-scale)) ";\n"
    "gi->unit_dic[" (get-type) "].anim_speed = " (pr-str (x :anim-speed)) ";\n"
    "gi->unit_dic[" (get-type) "].bar_height = " (pr-str (x :bar-height)) ";\n"
    "gi->unit_dic[" (get-type) "].hp_bar_size = " (pr-str (x :hp-bar-size)) ";\n"
    "gi->unit_dic[" (get-type) "].mn_bar_size = " (pr-str (x :mn-bar-size)) ";\n"))
;
;
(defn gen-unit-type [x]
  (str "TR_unit_type_" (x :name) ",\n"))
;
;
(defn gen-default-skill-type [x]
  (str
    "case TR_unit_type_" (x :name) ": {\n"
      "\treturn " (x :default-kill) ";\n"
    "}\n"))
;
;
(defn display-build-unit [x]
  (when (x :can-be-built?)
    (str "DISPLAY_BUILD_UNIT(\"(" (x :build-shortcut) ")\", " (x :name) ", '" (x :build-shortcut) "', \"" (x :info) "\")\n")))
;
;
(defn display-unlock-unit [x]
  (when (x :can-be-built?)
    (str "DISPLAY_UNLOCK_UNIT(\"(" (x :build-shortcut) ")\", " (x :name) ", " (x :unlock-xp) ", '" (x :build-shortcut) "', \"" (x :info) "\")\n")))
;
;
(defn gen-unit []
  (save-cpp (gen-on-desc unit-desc gen-get-unit-name) "gen-get-unit-name")
  (save-cpp (gen-on-desc unit-desc gen-on-attack-unit-sound) "gen-on-attack-unit-sound")
  (save-cpp (gen-on-desc unit-desc gen-on-move-unit-sound) "gen-on-move-unit-sound")
  (save-cpp (gen-on-desc unit-desc gen-on-select-unit-sound) "gen-on-select-unit-sound")
  (save-cpp (gen-on-desc unit-desc gen-play-dead-sound) "gen-play-dead-sound")
  (save-cpp (gen-on-desc unit-desc gen-sound-decl) "gen-sound-decl")
  (save-cpp (gen-on-desc unit-desc gen-sound-ini) "gen-sound-ini")
  (save-cpp (gen-on-desc unit-desc gen-sound-rel) "gen-sound-rel")
  (save-cpp (gen-on-desc unit-desc gen-unit-def) "gen-unit-def")
  (save-cpp (gen-on-desc unit-desc gen-unit-type) "gen-unit-type")
  (save-cpp (gen-on-desc unit-desc display-build-unit) "display-build-unit")
  (save-cpp (gen-on-desc unit-desc display-unlock-unit) "display-unlock-unit")
  (save-cpp (gen-on-desc unit-desc gen-default-skill-type) "gen-default-skill-type")
  
  (doall (map (fn [x] (check-sounds x)) unit-desc))
  (println "Units generated."))