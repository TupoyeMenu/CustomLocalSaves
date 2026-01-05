# CustomLocalSaves
A mod for Grand Theft Auto V Legacy and Enhanced that gives you control over your online saves.
Strictly for educational purposes.

## Features
### User editable json save files
The save files are located at `%appdata%/CLS`.
If you're planning on manually editing your save files you may want to enable
`use_human_readable_stat_names` in `%appdata%/CLS/settings.json`.
With this enabled your save files will use stat strings instead of stat hashes.

### Loading FSL saves
Loads FSL save files if you place them in `%appdata%/CLS`, and enable `load_fsl_files` in `%appdata%/CLS/settings.json`.
Note that FSL saves will only load if json saves were not found.
Saving in an FSL compatible format is NOT yet SUPPORTED.

### Stats that will be set on every load.
You may want certain stats to reset every time you load, for this create `%appdata%/CLS/save_overwrite.json`.
It uses the same syntax as the other save files, but it is never written to.
Here is an example `save_overwrite.json` that will refil your armor every time you load into GTA Online.
```json
{
	"BOOL": {},
	"DATE": {},
	"FLOAT": {},
	"INT": {
			"MP0_MP_CHAR_ARMOUR_1_COUNT": 10,
			"MP0_MP_CHAR_ARMOUR_2_COUNT": 10,
			"MP0_MP_CHAR_ARMOUR_3_COUNT": 10,
			"MP0_MP_CHAR_ARMOUR_4_COUNT": 10,
			"MP0_MP_CHAR_ARMOUR_5_COUNT": 10
	},
	"INT64": {},
	"PACKED": {},
	"POS": {},
	"STRING": {},
	"TEXTLABEL": {},
	"UINT16": {},
	"UINT32": {},
	"UINT64": {},
	"UINT8": {},
	"USERID": {},
	"uses_human_readable_stat_names": true
}
```

### GTA Online without R* services
CLS can load into GTA Online even if you don't have connection to R* services at the coast of breaking missions,
to enable this set `no_rgs` to `true` in `%appdata%/CLS/settings.json`.

## Usage
1. Install an ASI loader, I tested with [Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) as `bink2w64.dll`.
2. Put CLS.asi into your game directory, or wherever your asi loader will load from.
3. Run the game with a mod that has an anti-cheat bypass or use goldberg emulator.

## Building

### Windows
To build BigBaseV2 you need:

* Visual Studio 2019 or higher
* [CMake](https://cmake.org/download)

To set up the build environment, run the following commands in a terminal:
```bash
git clone https://github.com/TupoyeMenu/CustomLocalSaves.git
cd CustomLocalSaves
mkdir build
cd build
cmake ..
```
Now, you will be able to open the solution, and simply build it in Visual Studio.

### GNU/Linux
To build BigBaseV2 you need:

* MinGW 13 or higher
* CMake
* Ninja? Didn't test make.

To set up the build environment, run the following commands in a terminal:
```bash
git clone https://github.com/TupoyeMenu/CustomLocalSaves.git
cd CustomLocalSaves
mkdir build && cd build
cmake .. -DCROSSCOMPILE=ON -DUSE_GCC=ON -G Ninja
ninja
```

## Credits
| Credits                                                    |                                                                                                      |
| ---------------------------------------------------------- | ---------------------------------------------------------------------------------------------------- |
| Pocakking                                                  | [BigBaseV2](https://github.com/Pocakking/BigBaseV2)                                                  |
| spankerincrease(gir489)                                    | [BigBaseV2-fix](https://bitbucket.org/gir489/bigbasev2-fix)                                          |
| [YimMenu](https://github.com/YimMenu/YimMenu) contributors | Updates for newer GTA V versions, most of the code is stolen from them                               |
| FSL                                                        | Saved my save, help with profile stats, help with casino chips, create_native based native hook idea |
| tupoy-ya                                                   | Being stupid                                                                                         |

## License
This project is licensed under [GNU GENERAL PUBLIC LICENSE Version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt) because i stole a lot of code from YimMenu.
