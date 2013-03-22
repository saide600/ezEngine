﻿#include <PCH.h>

EZ_CREATE_SIMPLE_TEST(Strings, StringBuilder)
{
  EZ_TEST_BLOCK(true, "Constructor(empty)")
  {
    ezStringBuilder s;

    EZ_TEST(s.IsEmpty());
    EZ_TEST_INT(s.GetCharacterCount(), 0);
    EZ_TEST_INT(s.GetElementCount(), 0);
    EZ_TEST(s.IsPureASCII());
    EZ_TEST(s == "");
  }

  EZ_TEST_BLOCK(true, "Constructor(Utf8)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s(sUtf8.GetData());

    EZ_TEST(s.GetData() != sUtf8.GetData());
    EZ_TEST(s == sUtf8.GetData());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 13);
    EZ_TEST(!s.IsPureASCII());

    ezStringBuilder s2("test test");

    EZ_TEST(s2 == "test test");
    EZ_TEST_INT(s2.GetElementCount(), 9);
    EZ_TEST_INT(s2.GetCharacterCount(), 9);
    EZ_TEST(s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "Constructor(wchar_t)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s(L"abc äöü € def");

    EZ_TEST(s == sUtf8.GetData());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 13);
    EZ_TEST(!s.IsPureASCII());

    ezStringBuilder s2(L"test test");

    EZ_TEST(s2 == "test test");
    EZ_TEST_INT(s2.GetElementCount(), 9);
    EZ_TEST_INT(s2.GetCharacterCount(), 9);
    EZ_TEST(s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "Constructor(copy)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s(L"abc äöü € def");
    ezStringBuilder s2(s);

    EZ_TEST(s2 == sUtf8.GetData());
    EZ_TEST_INT(s2.GetElementCount(), 18);
    EZ_TEST_INT(s2.GetCharacterCount(), 13);
    EZ_TEST(!s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "Constructor(StringIterator)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");

    ezStringIterator it(sUtf8.GetData() + 2, sUtf8.GetData() + 8, sUtf8.GetData() + 2);

    ezStringBuilder s(it);

    EZ_TEST_INT(s.GetElementCount(), 6);
    EZ_TEST_INT(s.GetCharacterCount(), 4);
    EZ_TEST(!s.IsPureASCII());
    EZ_TEST(s == ezStringUtf8(L"c äö").GetData());
  }

  EZ_TEST_BLOCK(true, "operator=(Utf8)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s("bla");
    s = sUtf8.GetData();

    EZ_TEST(s.GetData() != sUtf8.GetData());
    EZ_TEST(s == sUtf8.GetData());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 13);
    EZ_TEST(!s.IsPureASCII());

    ezStringBuilder s2("bla");
    s2 = "test test";

    EZ_TEST(s2 == "test test");
    EZ_TEST_INT(s2.GetElementCount(), 9);
    EZ_TEST_INT(s2.GetCharacterCount(), 9);
    EZ_TEST(s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "operator=(wchar_t)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s("bla");
    s = L"abc äöü € def";

    EZ_TEST(s == sUtf8.GetData());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 13);
    EZ_TEST(!s.IsPureASCII());

    ezStringBuilder s2("bla");
    s2 = L"test test";

    EZ_TEST(s2 == "test test");
    EZ_TEST_INT(s2.GetElementCount(), 9);
    EZ_TEST_INT(s2.GetCharacterCount(), 9);
    EZ_TEST(s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "operator=(copy)")
  {
    ezStringUtf8 sUtf8(L"abc äöü € def");
    ezStringBuilder s(L"abc äöü € def");
    ezStringBuilder s2;
    s2 = s;

    EZ_TEST(s2 == sUtf8.GetData());
    EZ_TEST_INT(s2.GetElementCount(), 18);
    EZ_TEST_INT(s2.GetCharacterCount(), 13);
    EZ_TEST(!s2.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "operator=(StringIterator)")
  {
    ezStringBuilder s ("abcdefghi");
    ezStringIterator it(s.GetData() + 2, s.GetData() + 8, s.GetData() + 3);

    s = it;

    EZ_TEST(s == "defgh");
    EZ_TEST_INT(s.GetElementCount(), 5);
    EZ_TEST_INT(s.GetCharacterCount(), 5);
  }

  EZ_TEST_BLOCK(true, "Clear")
  {
    ezStringBuilder s(L"abc äöü € def");

    EZ_TEST(!s.IsEmpty());
    EZ_TEST(!s.IsPureASCII());

    s.Clear();
    EZ_TEST(s.IsEmpty());
    EZ_TEST_INT(s.GetElementCount(), 0);
    EZ_TEST_INT(s.GetCharacterCount(), 0);
    EZ_TEST(s.IsPureASCII());
  }

  EZ_TEST_BLOCK(true, "GetElementCount / GetCharacterCount / IsPureASCII")
  {
    ezStringBuilder s(L"abc äöü € def");

    EZ_TEST(!s.IsPureASCII());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 13);

    s = "abc";

    EZ_TEST(s.IsPureASCII());
    EZ_TEST_INT(s.GetElementCount(), 3);
    EZ_TEST_INT(s.GetCharacterCount(), 3);

    s = L"Hällo! I love €";

    EZ_TEST(!s.IsPureASCII());
    EZ_TEST_INT(s.GetElementCount(), 18);
    EZ_TEST_INT(s.GetCharacterCount(), 15);
  }

  EZ_TEST_BLOCK(true, "Append(char)")
  {
    ezStringBuilder s("abc");
    s.Append("de", "fg", "hi", ezStringUtf8(L"öä").GetData(), "jk", ezStringUtf8(L"ü€").GetData());

    EZ_TEST(s == ezStringUtf8(L"abcdefghiöäjkü€").GetData());

    s = "pups";
    s.Append(NULL, "b", NULL, "d", NULL, ezStringUtf8(L"ü€").GetData());
    EZ_TEST(s == ezStringUtf8(L"pupsbdü€").GetData());
  }

  EZ_TEST_BLOCK(true, "Append(wchar_t)")
  {
    ezStringBuilder s("abc");
    s.Append(L"de", L"fg", L"hi", L"öä", L"jk", L"ü€");

    EZ_TEST(s == ezStringUtf8(L"abcdefghiöäjkü€").GetData());

    s = "pups";
    s.Append(NULL, L"b", NULL, L"d", NULL, L"ü€");
    EZ_TEST(s == ezStringUtf8(L"pupsbdü€").GetData());
  }

  EZ_TEST_BLOCK(true, "AppendFormat")
  {
    ezStringBuilder s("abc");
    s.AppendFormat("Test%i%s%s", 42, "foo", ezStringUtf8(L"bär").GetData());

    EZ_TEST(s == ezStringUtf8(L"abcTest42foobär").GetData());
  }

  EZ_TEST_BLOCK(true, "Prepend(char)")
  {
    ezStringBuilder s("abc");
    s.Prepend("de", "fg", "hi", ezStringUtf8(L"öä").GetData(), "jk", ezStringUtf8(L"ü€").GetData());

    EZ_TEST(s == ezStringUtf8(L"defghiöäjkü€abc").GetData());

    s = "pups";
    s.Prepend(NULL, "b", NULL, "d", NULL, ezStringUtf8(L"ü€").GetData());
    EZ_TEST(s == ezStringUtf8(L"bdü€pups").GetData());
  }

  EZ_TEST_BLOCK(true, "Prepend(wchar_t)")
  {
    ezStringBuilder s("abc");
    s.Prepend(L"de", L"fg", L"hi", L"öä", L"jk", L"ü€");

    EZ_TEST(s == ezStringUtf8(L"defghiöäjkü€abc").GetData());

    s = "pups";
    s.Prepend(NULL, L"b", NULL, L"d", NULL, L"ü€");
    EZ_TEST(s == ezStringUtf8(L"bdü€pups").GetData());
  }

  EZ_TEST_BLOCK(true, "PrependFormat")
  {
    ezStringBuilder s("abc");
    s.PrependFormat("Test%i%s%s", 42, "foo", ezStringUtf8(L"bär").GetData());

    EZ_TEST(s == ezStringUtf8(L"Test42foobärabc").GetData());
  }


  EZ_TEST_BLOCK(true, "Format")
  {
    ezStringBuilder s("abc");
    s.Format("Test%i%s%s", 42, "foo", ezStringUtf8(L"bär").GetData());

    EZ_TEST(s == ezStringUtf8(L"Test42foobär").GetData());
  }

  EZ_TEST_BLOCK(true, "ToUpper")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    s.ToUpper();
    EZ_TEST(s == ezStringUtf8(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ€ß").GetData());
  }

  EZ_TEST_BLOCK(true, "ToLower")
  {
    ezStringBuilder s(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ€ß");
    s.ToLower();
    EZ_TEST(s == ezStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData());
  }

  EZ_TEST_BLOCK(true, "Shrink")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    s.Shrink(5, 3);

    EZ_TEST(s == ezStringUtf8(L"fghijklmnopqrstuvwxyzäö").GetData());

    s.Shrink(9, 7);
    EZ_TEST(s == ezStringUtf8(L"opqrstu").GetData());

    s.Shrink(3, 2);
    EZ_TEST(s == ezStringUtf8(L"rs").GetData());

    s.Shrink(1, 0);
    EZ_TEST(s == ezStringUtf8(L"s").GetData());

    s.Shrink(0, 0);
    EZ_TEST(s == ezStringUtf8(L"s").GetData());

    s.Shrink(0, 1);
    EZ_TEST(s == ezStringUtf8(L"").GetData());

    s.Shrink(10, 0);
    EZ_TEST(s == ezStringUtf8(L"").GetData());

    s.Shrink(0, 10);
    EZ_TEST(s == ezStringUtf8(L"").GetData());
  }

  EZ_TEST_BLOCK(true, "GetIteratorFront")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    ezStringIterator it = s.GetIteratorFront();

    EZ_TEST(it.StartsWith(ezStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));
    EZ_TEST(it.EndsWith(ezStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));

    it.ResetToBack();

    EZ_TEST(it.StartsWith(ezStringUtf8(L"ß").GetData()));
    EZ_TEST(it.EndsWith(ezStringUtf8(L"ß").GetData()));
  }

  EZ_TEST_BLOCK(true, "GetIteratorBack")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    ezStringIterator it = s.GetIteratorBack();

    EZ_TEST(it.StartsWith(ezStringUtf8(L"ß").GetData()));
    EZ_TEST(it.EndsWith(ezStringUtf8(L"ß").GetData()));

    it.ResetToFront();

    EZ_TEST(it.StartsWith(ezStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));
    EZ_TEST(it.EndsWith(ezStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));
  }

  EZ_TEST_BLOCK(true, "ChangeCharacter")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");

    ezStringUtf8 upr(L"ÄÖÜ€ßABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ezStringIterator it2(upr.GetData());
    
    for (ezStringIterator it = s.GetIteratorFront(); it.IsValid(); ++it, ++it2)
    {
      s.ChangeCharacter(it, it2.GetCharacter());
      
      EZ_TEST(it.GetCharacter() == it2.GetCharacter()); // iterator reflects the changes
    }

    EZ_TEST(s == upr.GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 31);
    EZ_TEST_INT(s.GetElementCount(), 37);
  }

  EZ_TEST_BLOCK(true, "ReplaceSubString")
  {
    ezStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");

    s.ReplaceSubString(s.GetData() + 3, s.GetData() + 7, "DEFG"); // equal length, equal num characters
    EZ_TEST(s == ezStringUtf8(L"abcDEFGhijklmnopqrstuvwxyzäöü€ß").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 31);
    EZ_TEST_INT(s.GetElementCount(), 37);

    s.ReplaceSubString(s.GetData() + 7, s.GetData() + 15, ""); // remove
    EZ_TEST(s == ezStringUtf8(L"abcDEFGpqrstuvwxyzäöü€ß").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 23);
    EZ_TEST_INT(s.GetElementCount(), 29);

    s.ReplaceSubString(s.GetData() + 17, s.GetData() + 22, "blablub"); // make longer
    EZ_TEST(s == ezStringUtf8(L"abcDEFGpqrstuvwxyblablubü€ß").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 27);
    EZ_TEST_INT(s.GetElementCount(), 31);

    s.ReplaceSubString(s.GetData() + 22, s.GetData() + 22, ezStringUtf8(L"määh!").GetData()); // insert
    EZ_TEST(s == ezStringUtf8(L"abcDEFGpqrstuvwxyblablmääh!ubü€ß").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 32);
    EZ_TEST_INT(s.GetElementCount(), 38);

    s.ReplaceSubString(s.GetData(), s.GetData() + 10, NULL); // remove at front
    EZ_TEST(s == ezStringUtf8(L"stuvwxyblablmääh!ubü€ß").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 22);
    EZ_TEST_INT(s.GetElementCount(), 28);

    s.ReplaceSubString(s.GetData() + 18, s.GetData() + 28, NULL); // remove at back
    EZ_TEST(s == ezStringUtf8(L"stuvwxyblablmääh").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 16);
    EZ_TEST_INT(s.GetElementCount(), 18);

    s.ReplaceSubString(s.GetData(), s.GetData() + 18, NULL); // clear
    EZ_TEST(s == ezStringUtf8(L"").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 0);
    EZ_TEST_INT(s.GetElementCount(), 0);

    const char* szInsert = "abc def ghi";

    s.ReplaceSubString(s.GetData(), s.GetData(), szInsert, szInsert + 7); // partial insert into empty
    EZ_TEST(s == ezStringUtf8(L"abc def").GetData());
    EZ_TEST_INT(s.GetCharacterCount(), 7);
    EZ_TEST_INT(s.GetElementCount(), 7);
  }

  EZ_TEST_BLOCK(true, "Insert")
  {
    ezStringBuilder s;

    s.Insert(s.GetData(), "test");
    EZ_TEST(s == "test");

    s.Insert(s.GetData() + 2, "TUT");
    EZ_TEST(s == "teTUTst");

    s.Insert(s.GetData(), "MOEP");
    EZ_TEST(s == "MOEPteTUTst");

    s.Insert(s.GetData() + s.GetElementCount(), "hompf");
    EZ_TEST(s == "MOEPteTUTsthompf");
  }

  EZ_TEST_BLOCK(true, "Remove")
  {
    ezStringBuilder s("MOEPteTUTsthompf");

    s.Remove(s.GetData() + 11, s.GetData() + s.GetElementCount());
    EZ_TEST(s == "MOEPteTUTst");

    s.Remove(s.GetData(), s.GetData() + 4);
    EZ_TEST(s == "teTUTst");

    s.Remove(s.GetData() + 2, s.GetData() + 5);
    EZ_TEST(s == "test");

    s.Remove(s.GetData(), s.GetData() + s.GetElementCount());
    EZ_TEST(s == "");
  }

  EZ_TEST_BLOCK(true, "ReplaceFirst")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceFirst("def", "BLOED");
    EZ_TEST(s == "abc BLOED abc def ghi abc ghi");

    s.ReplaceFirst("abc", "BLOED");
    EZ_TEST(s == "BLOED BLOED abc def ghi abc ghi");

    s.ReplaceFirst("abc", "BLOED", s.GetData() + 15);
    EZ_TEST(s == "BLOED BLOED abc def ghi BLOED ghi");

    s.ReplaceFirst("ghi", "LAANGWEILIG");
    EZ_TEST(s == "BLOED BLOED abc def LAANGWEILIG BLOED ghi");

    s.ReplaceFirst("ghi", "LAANGWEILIG");
    EZ_TEST(s == "BLOED BLOED abc def LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("def", "OEDE");
    EZ_TEST(s == "BLOED BLOED abc OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("abc", "BLOEDE");
    EZ_TEST(s == "BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG", "weg");
    EZ_TEST(s == "weg");

    s.ReplaceFirst("weg", NULL);
    EZ_TEST(s == "");
  }

  EZ_TEST_BLOCK(true, "ReplaceLast")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceLast("abc", "ABC");
    EZ_TEST(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast("abc", "ABC");
    EZ_TEST(s == "abc def ABC def ghi ABC ghi");

    s.ReplaceLast("abc", "ABC");
    EZ_TEST(s == "ABC def ABC def ghi ABC ghi");

    s.ReplaceLast("ghi", "GHI", s.GetData() + 24);
    EZ_TEST(s == "ABC def ABC def GHI ABC ghi");

    s.ReplaceLast("i", "I");
    EZ_TEST(s == "ABC def ABC def GHI ABC ghI");
  }

  EZ_TEST_BLOCK(true, "ReplaceAll")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceAll("abc", "TEST");
    EZ_TEST(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll("def", "def");
    EZ_TEST(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll("def", "defdef");
    EZ_TEST(s == "TEST defdef TEST defdef ghi TEST ghi");

    s.ReplaceAll("def", "defdef");
    EZ_TEST(s == "TEST defdefdefdef TEST defdefdefdef ghi TEST ghi");

    s.ReplaceAll("def", " ");
    EZ_TEST(s == "TEST      TEST      ghi TEST ghi");

    s.ReplaceAll(" ", "");
    EZ_TEST(s == "TESTTESTghiTESTghi");

    s.ReplaceAll("TEST", "a");
    EZ_TEST(s == "aaghiaghi");

    s.ReplaceAll("hi", "hihi");
    EZ_TEST(s == "aaghihiaghihi");

    s.ReplaceAll("ag", " ");
    EZ_TEST(s == "a hihi hihi");
  }

  EZ_TEST_BLOCK(true, "ReplaceFirst_NoCase")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceFirst_NoCase("DEF", "BLOED");
    EZ_TEST(s == "abc BLOED abc def ghi abc ghi");

    s.ReplaceFirst_NoCase("ABC", "BLOED");
    EZ_TEST(s == "BLOED BLOED abc def ghi abc ghi");

    s.ReplaceFirst_NoCase("ABC", "BLOED", s.GetData() + 15);
    EZ_TEST(s == "BLOED BLOED abc def ghi BLOED ghi");

    s.ReplaceFirst_NoCase("GHI", "LAANGWEILIG");
    EZ_TEST(s == "BLOED BLOED abc def LAANGWEILIG BLOED ghi");

    s.ReplaceFirst_NoCase("GHI", "LAANGWEILIG");
    EZ_TEST(s == "BLOED BLOED abc def LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("DEF", "OEDE");
    EZ_TEST(s == "BLOED BLOED abc OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("ABC", "BLOEDE");
    EZ_TEST(s == "BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG", "weg");
    EZ_TEST(s == "weg");

    s.ReplaceFirst_NoCase("WEG", NULL);
    EZ_TEST(s == "");
  }

  EZ_TEST_BLOCK(true, "ReplaceLast_NoCase")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceLast_NoCase("abc", "ABC");
    EZ_TEST(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("aBc", "ABC");
    EZ_TEST(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("ABC", "ABC");
    EZ_TEST(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("GHI", "GHI", s.GetData() + 24);
    EZ_TEST(s == "abc def abc def GHI ABC ghi");

    s.ReplaceLast_NoCase("I", "I");
    EZ_TEST(s == "abc def abc def GHI ABC ghI");
  }

  EZ_TEST_BLOCK(true, "ReplaceAll_NoCase")
  {
    ezStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceAll_NoCase("ABC", "TEST");
    EZ_TEST(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "def");
    EZ_TEST(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "defdef");
    EZ_TEST(s == "TEST defdef TEST defdef ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "defdef");
    EZ_TEST(s == "TEST defdefdefdef TEST defdefdefdef ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", " ");
    EZ_TEST(s == "TEST      TEST      ghi TEST ghi");

    s.ReplaceAll_NoCase(" ", "");
    EZ_TEST(s == "TESTTESTghiTESTghi");

    s.ReplaceAll_NoCase("teST", "a");
    EZ_TEST(s == "aaghiaghi");

    s.ReplaceAll_NoCase("hI", "hihi");
    EZ_TEST(s == "aaghihiaghihi");

    s.ReplaceAll_NoCase("Ag", " ");
    EZ_TEST(s == "a hihi hihi");
  }

  EZ_TEST_BLOCK(true, "ReplaceWholeWord")
  {
    ezStringBuilder s = "abcd abc abcd abc dabc abc";

    EZ_TEST(s.ReplaceWholeWord("abc", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd abc dabc abc");

    EZ_TEST(s.ReplaceWholeWord("abc", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd def dabc abc");

    EZ_TEST(s.ReplaceWholeWord("abc", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord("abc", "def", ezStringUtils::IsWordDelimiter_English) == NULL);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord("abcd", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "def def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord("abcd", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "def def def def dabc def");

    EZ_TEST(s.ReplaceWholeWord("abcd", "def", ezStringUtils::IsWordDelimiter_English) == NULL);
    EZ_TEST(s == "def def def def dabc def");
  }

  EZ_TEST_BLOCK(true, "ReplaceWholeWord_NoCase")
  {
    ezStringBuilder s = "abcd abc abcd abc dabc abc";

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd abc dabc abc");

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd def dabc abc");

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English) == NULL);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABCd", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "def def abcd def dabc def");

    EZ_TEST(s.ReplaceWholeWord_NoCase("aBCD", "def", ezStringUtils::IsWordDelimiter_English) != NULL);
    EZ_TEST(s == "def def def def dabc def");

    EZ_TEST(s.ReplaceWholeWord_NoCase("ABcd", "def", ezStringUtils::IsWordDelimiter_English) == NULL);
    EZ_TEST(s == "def def def def dabc def");
  }

  EZ_TEST_BLOCK(true, "ReplaceWholeWordAll")
  {
    ezStringBuilder s = "abcd abc abcd abc dabc abc";

    EZ_TEST_INT(s.ReplaceWholeWordAll("abc", "def", ezStringUtils::IsWordDelimiter_English), 3);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll("abc", "def", ezStringUtils::IsWordDelimiter_English), 0);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll("abcd", "def", ezStringUtils::IsWordDelimiter_English), 2);
    EZ_TEST(s == "def def def def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll("abcd", "def", ezStringUtils::IsWordDelimiter_English), 0);
    EZ_TEST(s == "def def def def dabc def");
  }

  EZ_TEST_BLOCK(true, "ReplaceWholeWordAll_NoCase")
  {
    ezStringBuilder s = "abcd abc abcd abc dabc abc";

    EZ_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English), 3);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABC", "def", ezStringUtils::IsWordDelimiter_English), 0);
    EZ_TEST(s == "abcd def abcd def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABCd", "def", ezStringUtils::IsWordDelimiter_English), 2);
    EZ_TEST(s == "def def def def dabc def");

    EZ_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABCd", "def", ezStringUtils::IsWordDelimiter_English), 0);
    EZ_TEST(s == "def def def def dabc def");
  }

  EZ_TEST_BLOCK(true, "teset")
  {
    const char* sz = "abc def";
    ezStringIterator it(sz);

    ezStringBuilder s = it;
  }

  EZ_TEST_BLOCK(true, "Split")
  {
    ezStringBuilder s = "|abc,def<>ghi|,<>jkl|mno,pqr|stu";

    ezHybridArray<ezStringIterator, 32> SubStrings;

    s.Split(false, SubStrings, ",", "|", "<>");

    EZ_TEST_INT(SubStrings.GetCount(), 7);
    EZ_TEST(SubStrings[0] == "abc");
    EZ_TEST(SubStrings[1] == "def");
    EZ_TEST(SubStrings[2] == "ghi");
    EZ_TEST(SubStrings[3] == "jkl");
    EZ_TEST(SubStrings[4] == "mno");
    EZ_TEST(SubStrings[5] == "pqr");
    EZ_TEST(SubStrings[6] == "stu");

    s.Split(true, SubStrings, ",", "|", "<>");

    EZ_TEST_INT(SubStrings.GetCount(), 10);
    EZ_TEST(SubStrings[0] == "");
    EZ_TEST(SubStrings[1] == "abc");
    EZ_TEST(SubStrings[2] == "def");
    EZ_TEST(SubStrings[3] == "ghi");
    EZ_TEST(SubStrings[4] == "");
    EZ_TEST(SubStrings[5] == "");
    EZ_TEST(SubStrings[6] == "jkl");
    EZ_TEST(SubStrings[7] == "mno");
    EZ_TEST(SubStrings[8] == "pqr");
    EZ_TEST(SubStrings[9] == "stu");
  }


  EZ_TEST_BLOCK(true, "MakeCleanPath")
  {
    ezStringBuilder p;

    p = "C:\\test/test//tut";
    p.MakeCleanPath();
    EZ_TEST(p == "C:/test/test//tut");

    p = "\\test/test//tut\\\\";
    p.MakeCleanPath();
    EZ_TEST(p == "/test/test//tut//");

    p = "\\";
    p.MakeCleanPath();
    EZ_TEST(p == "/");

    p = "file";
    p.MakeCleanPath();
    EZ_TEST(p == "file");

    p = "C:\\test/..//tut";
    p.MakeCleanPath();
    EZ_TEST(p == "C://tut");

    p = "C:\\test/..";
    p.MakeCleanPath();
    EZ_TEST(p == "C:/test/..");

    p = "C:\\test/..\\";
    p.MakeCleanPath();
    EZ_TEST(p == "C:/");

    p = "\\//test/../bla\\\\blub///..\\test//tut/tat/..\\\\..\\//ploep";
    p.MakeCleanPath();
    EZ_TEST(p == "///bla//blub//test//tut///ploep");

    p = "a/b/c/../../../../e/f";
    p.MakeCleanPath();
    EZ_TEST(p == "../e/f");  

    p = "/../../a/../../e/f";
    p.MakeCleanPath();
    EZ_TEST(p == "../../e/f");

    p = "/../../a/../../e/f/../";
    p.MakeCleanPath();
    EZ_TEST(p == "../../e/");

    p = "/../../a/../../e/f/..";
    p.MakeCleanPath();
    EZ_TEST(p == "../../e/f/..");
  }

  EZ_TEST_BLOCK(true, "PathParentDirectory")
  {
    ezStringBuilder p;

    p = "C:\\test/test//tut";
    p.PathParentDirectory();
    EZ_TEST(p == "C:/test/test//");

    p = "C:\\test/test//tut\\\\";
    p.PathParentDirectory();
    EZ_TEST(p == "C:/test/test//tut/");

    p = "file";
    p.PathParentDirectory();
    EZ_TEST(p == "");

    p = "/file";
    p.PathParentDirectory();
    EZ_TEST(p == "/");

    p = "C:\\test/..//tut";
    p.PathParentDirectory();
    EZ_TEST(p == "C://");

    p = "file";
    p.PathParentDirectory(3);
    EZ_TEST(p == "../../");
  }

  EZ_TEST_BLOCK(true, "AppendPath")
  {
    ezStringBuilder p;

    p = "this/is\\my//path";
    p.AppendPath("orly/nowai");
    EZ_TEST(p == "this/is\\my//path/orly/nowai");

    p = "this/is\\my//path///";
    p.AppendPath("orly/nowai");
    EZ_TEST(p == "this/is\\my//path///orly/nowai");

    p = "";
    p.AppendPath("orly/nowai");
    EZ_TEST(p == "orly/nowai");
  
    p = "bla";
    p.AppendPath("");
    EZ_TEST(p == "bla");
  }

  EZ_TEST_BLOCK(true, "ChangeFileName")
  {
    ezStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileName("bla");
    EZ_TEST(p == "C:/test/test/bla.ext");

    p = "test/test/tut/troet.toeff";
    p.ChangeFileName("toeff");
    EZ_TEST(p == "test/test/tut/toeff.toeff");

    p = "test/test/tut/murpf";
    p.ChangeFileName("toeff");
    EZ_TEST(p == "test/test/tut/toeff");

    p = "test/test/tut/murpf/";
    p.ChangeFileName("toeff");
    EZ_TEST(p == "test/test/tut/murpf/toeff"); // filename is EMPTY -> thus ADDS it

    p = "test/test/tut/murpf/.extension"; // folders that start with a dot must be considered to be empty filenames with an extension
    p.ChangeFileName("toeff");
    EZ_TEST(p == "test/test/tut/murpf/toeff.extension");

    p = "test/test/tut/murpf/.extension/"; // folders that start with a dot ARE considered as folders, if the path ends with a slash
    p.ChangeFileName("toeff");
    EZ_TEST(p == "test/test/tut/murpf/.extension/toeff");
  }

  EZ_TEST_BLOCK(true, "ChangeFileNameAndExtension")
  {
    ezStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileNameAndExtension("bla.pups");
    EZ_TEST(p == "C:/test/test/bla.pups");

    p = "test/test/tut/troet.toeff";
    p.ChangeFileNameAndExtension("toeff");
    EZ_TEST(p == "test/test/tut/toeff");

    p = "test/test/tut/murpf";
    p.ChangeFileNameAndExtension("toeff.tut");
    EZ_TEST(p == "test/test/tut/toeff.tut");

    p = "test/test/tut/murpf/";
    p.ChangeFileNameAndExtension("toeff.blo");
    EZ_TEST(p == "test/test/tut/murpf/toeff.blo"); // filename is EMPTY -> thus ADDS it

    p = "test/test/tut/murpf/.extension"; // folders that start with a dot must be considered to be empty filenames with an extension
    p.ChangeFileNameAndExtension("toeff.ext");
    EZ_TEST(p == "test/test/tut/murpf/toeff.ext");

    p = "test/test/tut/murpf/.extension/"; // folders that start with a dot ARE considered as folders, if the path ends with a slash
    p.ChangeFileNameAndExtension("toeff");
    EZ_TEST(p == "test/test/tut/murpf/.extension/toeff");
  }

  EZ_TEST_BLOCK(true, "ChangeFileExtension")
  {
    ezStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileExtension("pups");
    EZ_TEST(p == "C:/test/test/tut.pups");

    p = "C:/test/test/tut";
    p.ChangeFileExtension("pups");
    EZ_TEST(p == "C:/test/test/tut.pups");

    p = "C:/test/test/tut.ext";
    p.ChangeFileExtension("");
    EZ_TEST(p == "C:/test/test/tut.");

    p = "C:/test/test/tut";
    p.ChangeFileExtension("");
    EZ_TEST(p == "C:/test/test/tut.");
  }

  EZ_TEST_BLOCK(true, "MakeAbsolutePath")
  {
    ezStringBuilder p = "../../d/..\\f";
    p.MakeAbsolutePath("C:\\a/b/c/");
    EZ_TEST(p == "C:/a/f");

  }

  EZ_TEST_BLOCK(true, "HasAnyExtension")
  {
    ezStringBuilder p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.HasAnyExtension());

    p = "This/Is\\My//Path.dot\\file_no_extension";
    EZ_TEST(!p.HasAnyExtension());
    EZ_TEST(!p.HasAnyExtension());
  }

  EZ_TEST_BLOCK(true, "HasExtension")
  {
    ezStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.HasExtension(".Extension"));

    p = "This/Is\\My//Path.dot\\file.ext";
    EZ_TEST(p.HasExtension("EXT"));

    p = "This/Is\\My//Path.dot\\file.ext";
    EZ_TEST(!p.HasExtension("NEXT"));

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(!p.HasExtension(".Ext"));

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(!p.HasExtension("sion"));

    p = "";
    EZ_TEST(!p.HasExtension("ext"));
  }

  EZ_TEST_BLOCK(true, "GetFileExtension")
  {
    ezStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.GetFileExtension() == "extension");

    p = "This/Is\\My//Path.dot\\file";
    EZ_TEST(p.GetFileExtension() == "");

    p = "";
    EZ_TEST(p.GetFileExtension() == "");
  }

  EZ_TEST_BLOCK(true, "GetFileNameAndExtension")
  {
    ezStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.GetFileNameAndExtension() == "file.extension");

    p = "This/Is\\My//Path.dot\\.extension";
    EZ_TEST(p.GetFileNameAndExtension() == ".extension");

    p = "This/Is\\My//Path.dot\\file";
    EZ_TEST(p.GetFileNameAndExtension() == "file");

    p = "\\file";
    EZ_TEST(p.GetFileNameAndExtension() == "file");

    p = "";
    EZ_TEST(p.GetFileNameAndExtension() == "");

    p = "/";
    EZ_TEST(p.GetFileNameAndExtension() == "");

    p = "This/Is\\My//Path.dot\\";
    EZ_TEST(p.GetFileNameAndExtension() == "");
  }

  EZ_TEST_BLOCK(true, "GetFileName")
  {
    ezStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.GetFileName() == "file");

    p = "This/Is\\My//Path.dot\\file";
    EZ_TEST(p.GetFileName() == "file");

    p = "\\file";
    EZ_TEST(p.GetFileName() == "file");

    p = "";
    EZ_TEST(p.GetFileName() == "");

    p = "/";
    EZ_TEST(p.GetFileName() == "");

    p = "This/Is\\My//Path.dot\\";
    EZ_TEST(p.GetFileName() == "");

    // so far we treat file and folders whose names start with a '.' as extensions
    p = "This/Is\\My//Path.dot\\.stupidfile";
    EZ_TEST(p.GetFileName() == "");
  }

  EZ_TEST_BLOCK(true, "GetFileDirectory")
  {
    ezStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    EZ_TEST(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This/Is\\My//Path.dot\\.extension";
    EZ_TEST(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This/Is\\My//Path.dot\\file";
    EZ_TEST(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "\\file";
    EZ_TEST(p.GetFileDirectory() == "\\");

    p = "";
    EZ_TEST(p.GetFileDirectory() == "");

    p = "/";
    EZ_TEST(p.GetFileDirectory() == "/");

    p = "This/Is\\My//Path.dot\\";
    EZ_TEST(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This";
    EZ_TEST(p.GetFileDirectory() == "");
  }

  EZ_TEST_BLOCK(true, "IsAbsolutePath / IsRelativePath")
  {
    ezStringBuilder p;

    #if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
      p = "C:\\test.stuff";
      EZ_TEST(p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath());

      p = "C:/test.stuff";
      EZ_TEST(p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath());

      p = "\\\\myserver\\test.stuff";
      EZ_TEST(p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath());

      p = "\\myserver\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath()); // neither absolute nor relativ, just stupid

      p = "test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());

      p = "/test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath()); // bloed

      p = "\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath()); // bloed

      p = "..\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());

      p = ".\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());

    #elif EZ_ENABLED(EZ_PLATFORM_OSX)
  
      p = "C:\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());
  
      p = "test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());
  
      p = "/test.stuff";
      EZ_TEST(p.IsAbsolutePath());
      EZ_TEST(!p.IsRelativePath());
  
      p = "..\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());
  
      p = ".\\test.stuff";
      EZ_TEST(!p.IsAbsolutePath());
      EZ_TEST(p.IsRelativePath());
  
    #elif EZ_ENABLED(EZ_PLATFORM_LINUX)
      #error Missing tests.
    #else
      #error Unknown platform.
    #endif
  }

  EZ_TEST_BLOCK(true, "IsPathBelowFolder")
  {
    ezStringBuilder p;

    p = "a/b\\c//d\\\\e/f";
    EZ_TEST(!p.IsPathBelowFolder("/a/b\\c"));
    EZ_TEST(p.IsPathBelowFolder("a/b\\c"));
    EZ_TEST(p.IsPathBelowFolder("a/b\\c//"));
    EZ_TEST(p.IsPathBelowFolder("a/b\\c//d/\\e\\f")); // equal paths are considered 'below'
    EZ_TEST(!p.IsPathBelowFolder("a/b\\c//d/\\e\\f/g"));
    EZ_TEST(p.IsPathBelowFolder("a"));
    EZ_TEST(!p.IsPathBelowFolder("b"));
  }

  EZ_TEST_BLOCK(true, "MakeRelativePath")
  {
    ezStringBuilder p;

    p = "a/b\\c/d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c");
    EZ_TEST(p == "d//e/f/g");

    p = "a/b\\c//d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c");
    EZ_TEST(p == "/d//e/f/g");

    p = "a/b\\c/d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c/");
    EZ_TEST(p == "d//e/f/g");

    p = "a/b\\c//d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c/");
    EZ_TEST(p == "/d//e/f/g");

    p = "a/b\\c//d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c\\/d/\\e\\f/g");
    EZ_TEST(p == "");

    p = "a/b\\c//d\\\\e/f/g/";
    p.MakeRelativePath("a\\b/c\\/d//e\\f/g\\h/i");
    EZ_TEST(p == "../../");

    p = "a/b\\c//d\\\\e/f/g/j/k";
    p.MakeRelativePath("a\\b/c\\/d//e\\f/g\\h/i");
    EZ_TEST(p == "../../j/k");

    p = "a/b\\c//d\\\\e/f/ge";
    p.MakeRelativePath("a\\b/c//d/\\e\\f/g\\h/i");
    EZ_TEST(p == "../../../ge");

    p = "a/b\\c//d\\\\e/f/g.txt";
    p.MakeRelativePath("a\\b/c//d//e\\f/g\\h/i");
    EZ_TEST(p == "../../../g.txt");

    p = "a/b\\c//d\\\\e/f/g";
    p.MakeRelativePath("a\\b/c//d//e\\f/g\\h/i");
    EZ_TEST(p == "../../");
  }

  EZ_TEST_BLOCK(true, "RemoveDoubleSlashesInPath")
  {
    ezStringBuilder p;

    p = "C:\\\\test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "C:/test.stuff");

    p = "C://test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "C:/test.stuff");

    p = "\\\\myserver\\test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "//myserver/test.stuff");

    p = "\\myserver/\\test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "/myserver/test.stuff");

    p = "test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "test.stuff");

    p = "/test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "/test.stuff");

    p = "\\/test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "//test.stuff");

    p = "..\\//test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "../test.stuff");

    p = ".\\\\test.stuff";
    p.RemoveDoubleSlashesInPath();
    EZ_TEST(p == "./test.stuff");
  }
}

