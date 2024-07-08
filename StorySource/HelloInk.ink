# use_dialogue_camera

EXTERNAL HasEgg()
EXTERNAL GiveEgg()

{HasEgg(): -> has_egg_dialogue | Take this egg from me, I can't take care of it. -> take_egg_choice}

=== take_egg_choice ===
+ [Take the egg] -> take_egg
+ [Refuse the egg] -> refuse_egg
+ [Egg?] -> what_is_egg

=== what_is_egg ===
Yeah, an egg.
This egg births a villager if you care for it properly.
But I don't want it, please take it.
-> take_egg_choice

=== refuse_egg ===
Alright!
I swear this egg is a wonderful responsibility, for the right person.
-> DONE

=== take_egg ===
You take the egg. {GiveEgg()}
This egg will hatch soon.
But what comes out depends on how well you take care of it.
Do you want me to tell you how to care for it?
++ [Yes]
Alright
-> egg_care
++ [No!]
I hope you know what you're doing!
If you ever need help, come back and i'll tell you what to do.
-> DONE

=== has_egg_dialogue ===
Thank you for taking the egg.
Do you want me to tell you how to take care of it?
++ [Yes]
Alright
-> egg_care
++ [No!]
I hope you know what you're doing!
If you ever need help, come back and i'll tell you what to do.
-> DONE

=== egg_care ===
The egg will hatch in three days.
I know its soon, but in that time, you need to satiate its needs.
It needs a warm house, food, water, and love.
If its neglected in any way, it may not come out of its shell.
Do you understand?
+ [Yes]
But do you really understand it? It will die if you can't care for it!
Can you blame it? I stayed in the womb as long as i could!
But that's all there is to it.
If you ever need help, come back and i'll tell you what to do.
-> DONE
* [No!]
Well let me repeat myself
-> egg_care