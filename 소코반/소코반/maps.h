#pragma once

int nowStage = 0;
const int maxStage = 2;

char ns[16][21];
char map[3][16][21] =
{
	{
		"####################",
		"####################",
		"####################",
		"#####   ############",
		"#####o  ############",
		"#####  o############",
		"###  o o ###########",
		"### # ## ###########",
		"#   # ## #####  ..##",
		"# o  o   @      ..##",
		"##### ### # ##  ..##",
		"#####     ##########",
		"####################",
		"####################",
		"####################",
		"####################"
	},
	{
		"####################",
		"####################",
		"####################",
		"####       #########",
		"#### ##### #########",
		"#### #####o#########",
		"#### #     #########",
		"#### # ###o#########",
		"#### # ### ###    ##",
		"### @     o    oo ##",
		"#########   ##    ##",
		"############### ..##",
		"###############..###",
		"############### .###",
		"####################",
		"####################"
	},
	{
		"####################",
		"####################",
		"##  ################",
		"##  ################",
		"###    #############",
		"###  # #############",
		"######    ##########",
		"######          .###",
		"##############. .###",
		"####################",
		"####################",
		"####################",
		"####################",
		"####################",
		"####################",
		"####################"
	}
};