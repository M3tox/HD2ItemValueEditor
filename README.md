# Usage
Simply start the editor, click on "File" and then open the desired items.sav file.
After loading, you can switch between the items via the top right drop down menu.
In the list below you can see the item values. You can change them as desired.
![grafik](https://github.com/M3tox/HD2ItemValueEditor/assets/75583358/f173ccda-c11f-4168-85a6-b3a1de342b69)

When done save the items.sav, or click on "Save as" to save a new file with a different file name.

To add whole NEW items, simply use one of the empty slots. H&D2 without the addon supports 255 items in total, but H&D2 with Sabre Squadron supports 500 items, which gives plenty of extra new slots.
Don't forget to change the record type from EMPTY to the desired form, otherwise you can't edit the necessary fields.

Not all fields are known. If you found something out or if you want me to correct some of the labels, simply contact me in discord or report it as an "Issue" here in github.

# Technical
If you are a programmer, feel free to use the code. cMain, cApp are used for the UI. The code uses the wxWidgets library, so the whole code is cross platform. If you only need something that can read the items.sav file, all you need is the Items.h and Items.cpp.

# Thank you

Special thanks to my friend Culticaxe who tested the tool. Also thanks to "RellHaiser" who independently worked on the items, too. 
