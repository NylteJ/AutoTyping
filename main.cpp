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

	println("�����ӳٱ�����������������˵�����ֵԽ�ߣ������ٶ�Խ���������õ�̫�Ϳ��ܻ��©�֡���ͬ��������޲�̫һ�£�����ϵͳ�Դ����±������޴�Լ�� 15���� VSCode �������Ե��� 1������ 0 ��������ģʽ����");

	unsigned long long delay;

	cin >> delay;

	if (delay == 0)
		println("����������ģʽ�������Ծ����ܿ���ٶ����У�������ȡ���ӳ٣���ȫ���ӳ�ʱ����һ�������֣���");

	println("����Ҫ����������� (��;�ɻ��У��������������У�������ɺ��������м���)��");

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
		println("���棺����������а����� ASCII �ַ������������ͼת��һ����ȫ���ַ����������޷�ת�����ַ���");

		// TODO: ��һ�������ŵĽ������
		const vector replaceList = {
		pair{ "��"sv, ", "sv },
			{ "��"sv, ". "sv },
			{ "��"sv, "? "sv },
			{ "��"sv, "! "sv },
			{ "��"sv, ": "sv },
			{ "��"sv, "; "sv },
			{ "��"sv, "\""sv },
			{ "��"sv, "\""sv },
			{ "��"sv, "'"sv },
			{ "��"sv, "'"sv },
			{ "��"sv, "("sv },
			{ "��"sv, ")"sv },
			{ "��"sv, "["sv },
			{ "��"sv, "]"sv },
			{ "��"sv, "<"sv },
			{ "��"sv, ">"sv },
			{ "��"sv, ", "sv },
			{ "����"sv, "..."sv },
			{ "��"sv, " - "sv }};

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
			println("�滻��ϣ����з� ASCII �ַ����õ����滻������Ӧ�ò�������⡣");
		else
			println("�滻��ϣ������з� ASCII �ַ����ڣ����򽫺�����Щ�ַ��������ֶ�������벢�滻�ⲿ���ַ���");

		println("Ϊ�����㿴���⼸���֣������ӳ� 5 �롣\n");
		this_thread::sleep_for(5s);
	}

	println("׼����ʼ���֣��ӳ� 3 ���ʼ......");

	this_thread::sleep_for(3s);

	typer.Type(input, 1ms * delay);

	return 0;
}