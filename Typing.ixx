module;
#include <Windows.h>
#include <WinUser.h>

export module Typing;

import std;

using namespace std;

export namespace NylteJ
{
	class Typer
	{
	private:
		using KeyPosition = pair<short, short>;
		using KeyPosMap = unordered_map<char, KeyPosition>;

		using Key = BYTE;
	private:
		static KeyPosMap delayMap;
	private:
		void KeyDown(Key key)
		{
			INPUT input{ .type = INPUT_KEYBOARD,.ki = {.wVk = key} };

			SendInput(1, &input, sizeof(INPUT));
		}
		void KeyUp(Key key)
		{
			INPUT input{ .type = INPUT_KEYBOARD,.ki = {.wVk = key,.dwFlags = KEYEVENTF_KEYUP} };

			SendInput(1, &input, sizeof(INPUT));
		}
		void KeyPress(Key key)
		{
			array<INPUT, 2> inputs{ INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = key} },
									INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = key,.dwFlags = KEYEVENTF_KEYUP} } };

			SendInput(2, inputs.data(), sizeof(INPUT));
		}
		void KeyPressWithShift(Key key)
		{
			array<INPUT, 4> inputs{ INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = VK_SHIFT} },
									INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = key} },
									INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = key,.dwFlags = KEYEVENTF_KEYUP} },
									INPUT{.type = INPUT_KEYBOARD,.ki = {.wVk = VK_SHIFT,.dwFlags = KEYEVENTF_KEYUP} } };

			SendInput(4, inputs.data(), sizeof(INPUT));
		}

		constexpr bool NeedShift(char chr)
		{
			constexpr string_view needShiftChrs = "~!@#$%^&*()_+{}:\"<>?|"sv;
			return needShiftChrs.contains(chr) || (chr >= 'A' && chr <= 'Z');
		}
	public:
		void Type(char chr)
		{
			if (chr < 0 || chr > 255)
				return;

			if (isupper(chr))
				KeyPressWithShift(chr);
			else if (islower(chr))
				KeyPress(toupper(chr));
			else if (chr >= '0' && chr <= '9')
				KeyPress(chr);
			else
			{
				Key newKey;
				switch (chr)
				{
				case '=':
				case '+':newKey = VK_OEM_PLUS; break;
				case ',':
				case '<':newKey = VK_OEM_COMMA; break;
				case '-':
				case '_':newKey = VK_OEM_MINUS; break;
				case '.':
				case '>':newKey = VK_OEM_PERIOD; break;
				case ';':
				case ':':newKey = VK_OEM_1; break;
				case '/':
				case '?':newKey = VK_OEM_2; break;
				case '`':
				case '~':newKey = VK_OEM_3; break;
				case '[':
				case '{':newKey = VK_OEM_4; break;
				case '\\':
				case '|':newKey = VK_OEM_5; break;
				case ']':
				case '}':newKey = VK_OEM_6; break;
				case '\'':
				case '"':newKey = VK_OEM_7; break;

				case '\n':newKey = VK_RETURN; break;
				case ' ':newKey = VK_SPACE; break;
				case '\t':newKey = VK_TAB; break;

				case '!':newKey = '1'; break;
				case '@':newKey = '2'; break;
				case '#':newKey = '3'; break;
				case '$':newKey = '4'; break;
				case '%':newKey = '5'; break;
				case '^':newKey = '6'; break;
				case '&':newKey = '7'; break;
				case '*':newKey = '8'; break;
				case '(':newKey = '9'; break;
				case ')':newKey = '0'; break;

				default:return;
				}

				if (NeedShift(chr))
					KeyPressWithShift(newKey);
				else
					KeyPress(newKey);
			}
		}
		void Type(string_view text, chrono::milliseconds delayRatio)
		{
			for (int i = 0; i < text.size(); i++)
			{
				if (i > 0 && delayMap.contains(text[i]) && delayMap.contains(text[i - 1]))
				{
					constexpr static auto GetDistance = [](auto&& posA, auto&& posB) constexpr
						{
							return abs(posA.first - posB.first) + abs(posA.second - posB.second);
						};

					auto distance = GetDistance(delayMap[text[i]], delayMap[text[i - 1]]);

					if (NeedShift(text[i]))
						distance += min(GetDistance(delayMap[text[i]], delayMap[VK_SHIFT]), 4);

					this_thread::sleep_for(distance * delayRatio);
				}
				else
					this_thread::sleep_for(4 * delayRatio);

				if (delayRatio == 0ms)
				{
					if (text[i] == '\n')
						this_thread::sleep_for(100ms);
					if (i % 20 == 0)
						this_thread::sleep_for(50ms);	// 和分 20 次 sleep 不一样，后者由于时间片切换的关系，实际 sleep 时间远长于前者
				}

				Type(text[i]);

				if (delayRatio == 0ms && text[i] == '\n')
					this_thread::sleep_for(100ms);
			}
		}
	};

	// 存储的应当是 “要打出来的字符”，而不是 “要按下的键”，除了个别特殊字符（比如 Shift）
	unordered_map<char, pair<short, short>> Typer::delayMap = []
		{
			unordered_map<char, pair<short, short>> ret;

			ret['`'] = ret['~'] = { -1,-1 };

			ret['1'] = ret['!'] = { -1,0 };
			ret['2'] = ret['@'] = { -1,1 };
			ret['3'] = ret['#'] = { -1,2 };
			ret['4'] = ret['$'] = { -1,3 };
			ret['5'] = ret['%'] = { -1,4 };
			ret['6'] = ret['^'] = { -1,5 };
			ret['7'] = ret['&'] = { -1,6 };
			ret['8'] = ret['*'] = { -1,7 };
			ret['9'] = ret['('] = { -1,8 };
			ret['0'] = ret[')'] = { -1,9 };
			ret['-'] = ret['_'] = { -1,10 };
			ret['='] = ret['+'] = { -1,11 };

			ret['\t'] = { 0,-1 };

			ret['q'] = ret['Q'] = { 0,0 };
			ret['w'] = ret['W'] = { 0,1 };
			ret['e'] = ret['E'] = { 0,2 };
			ret['r'] = ret['R'] = { 0,3 };
			ret['t'] = ret['T'] = { 0,4 };
			ret['y'] = ret['Y'] = { 0,5 };
			ret['u'] = ret['U'] = { 0,6 };
			ret['i'] = ret['I'] = { 0,7 };
			ret['o'] = ret['O'] = { 0,8 };
			ret['p'] = ret['P'] = { 0,9 };
			ret['['] = ret['{'] = { 0,10 };
			ret[']'] = ret['}'] = { 0,11 };

			ret['a'] = ret['A'] = { 1,0 };
			ret['s'] = ret['S'] = { 1,1 };
			ret['d'] = ret['D'] = { 1,2 };
			ret['f'] = ret['F'] = { 1,3 };
			ret['g'] = ret['G'] = { 1,4 };
			ret['h'] = ret['H'] = { 1,5 };
			ret['j'] = ret['J'] = { 1,6 };
			ret['k'] = ret['K'] = { 1,7 };
			ret['l'] = ret['L'] = { 1,8 };
			ret[';'] = ret[':'] = { 1,9 };
			ret['\''] = ret['"'] = { 1,10 };
			ret['\n'] = { 1,11 };

			ret[VK_SHIFT] = { 2,-1 };	// 占位

			ret['z'] = ret['Z'] = { 2,0 };
			ret['x'] = ret['X'] = { 2,1 };
			ret['c'] = ret['C'] = { 2,2 };
			ret['v'] = ret['V'] = { 2,3 };
			ret['b'] = ret['B'] = { 2,4 };
			ret['n'] = ret['N'] = { 2,5 };
			ret['m'] = ret['M'] = { 2,6 };
			ret[','] = ret['<'] = { 2,7 };
			ret['.'] = ret['>'] = { 2,8 };
			ret['/'] = ret['?'] = { 2,9 };

			ret[' '] = { 3,4 };	// 盲估的

			return ret;
		}();
}