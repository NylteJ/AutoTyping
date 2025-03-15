import std;

import Typing;

using namespace std;
using namespace NylteJ;

void Replace(string& str, string_view target, string_view replacement)
{
	size_t pos = 0;
	while ((pos = str.find(target, pos)) != string::npos)
	{
		str.replace(pos, target.length(), replacement);
		pos += replacement.length();
	}
}

int main()
{
	Typer typer;

	println("输入延迟比例（正整数。简单来说，这个值越高，打字速度越慢，但设置得太低可能会打漏字。不同软件的下限不太一致，比如系统自带记事本的下限大约是 15，但 VSCode 甚至可以调到 1。输入 0 启动急速模式）：");

	unsigned long long delay;

	cin >> delay;

	if (delay == 0)
		println("已启动急速模式，程序将以尽可能快的速度运行（并不会取消延迟，完全不延迟时几乎一定会打错字）。");

	println("输入要打的文字内容 (中途可换行，但不能连换三行；输入完成后连续换行即可)：");

	string input;
	{
		string temp;
		bool lastisReturn = false;

		while(true)
		{
			getline(cin, temp);

			input += temp + '\n';

			if (temp.empty() && lastisReturn)
				break;
			if (temp.empty())
				lastisReturn = true;
			else
				lastisReturn = false;
		}

		input = input
			| ranges::views::drop_while([](char chr) {return chr == '\n'; })
			| ranges::views::reverse
			| ranges::views::drop_while([](char chr) {return chr == '\n'; })
			| ranges::views::reverse
			| ranges::to<string>();
	}

	bool allASCII = true;
	for (char chr : input)
		if (chr < 0 || chr>255)
		{
			allASCII = false;
			break;
		}

	if (!allASCII)
	{
		println("警告：输入的内容中包含非 ASCII 字符，程序会先试图转化一部分全角字符，并忽略无法转换的字符。");

		// TODO: 想一个更优雅的解决方案
		const vector replaceList = {
		pair{ "，"sv, ", "sv },
			{ "。"sv, ". "sv },
			{ "？"sv, "? "sv },
			{ "！"sv, "! "sv },
			{ "："sv, ": "sv },
			{ "；"sv, "; "sv },
			{ "“"sv, "\""sv },
			{ "”"sv, "\""sv },
			{ "‘"sv, "'"sv },
			{ "’"sv, "'"sv },
			{ "（"sv, "("sv },
			{ "）"sv, ")"sv },
			{ "【"sv, "["sv },
			{ "】"sv, "]"sv },
			{ "《"sv, "<"sv },
			{ "》"sv, ">"sv },
			{ "、"sv, ", "sv },
			{ "……"sv, "..."sv },
			{ "—"sv, " - "sv }};

		for (auto&& [target, replacement] : replaceList)
			Replace(input, target, replacement);

		allASCII = true;
		for (char chr : input)
			if (chr < 0 || chr > 255)
			{
				allASCII = false;
				break;
			}

		if (allASCII)
			println("替换完毕，所有非 ASCII 字符都得到了替换，所以应该不会出问题。");
		else
			println("替换完毕，但仍有非 ASCII 字符存在，程序将忽略这些字符。建议手动检查输入并替换这部分字符。");

		println("为了让你看清这几行字，程序将延迟 5 秒。\n");
		this_thread::sleep_for(5s);
	}

	println("准备开始打字，延迟 3 秒后开始......");

	this_thread::sleep_for(3s);

	typer.Type(input, 1ms * delay);

	return 0;
}