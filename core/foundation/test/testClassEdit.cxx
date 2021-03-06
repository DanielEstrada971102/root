#include "TClassEdit.h"

#include "gtest/gtest.h"

/*
/// Return type of the function, might be empty if the function declaration string did not provide it.
std::string fReturnType;

/// Name of the scope qualification of the function, possibly empty
std::string fScopeName;

/// Name of the function
std::string fFunctionName;

/// Template arguments of the function template specialization, if any; will contain one element "" for
/// `function<>()`
std::vector<std::string> fFunctionTemplateArguments;

/// Function parameters.
std::vector<std::string> fFunctionParameters;
*/

TEST(TClassEdit, SplitFunc)
{
   TClassEdit::FunctionSplitInfo fsi;

   TClassEdit::SplitFunction("", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("bar", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("bar", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("bar()", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("bar", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo bar", fsi);
   EXPECT_EQ("foo", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("bar", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo bar baz()", fsi);
   EXPECT_EQ("foo bar", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("baz", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo::baz()", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("foo", fsi.fScopeName);
   EXPECT_EQ("baz", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo<int>::baz()", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("foo<int>", fsi.fScopeName);
   EXPECT_EQ("baz", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("bar<foo>", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("bar", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("foo", fsi.fFunctionTemplateArguments[0]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("bar<>()", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("bar", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("", fsi.fFunctionTemplateArguments[0]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("bar<foo> foo<bar>", fsi);
   EXPECT_EQ("bar<foo>", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("bar", fsi.fFunctionTemplateArguments[0]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("abc<xyz>::bar<foo> cde::fgh<ijk>::foo<bar>", fsi);
   EXPECT_EQ("abc<xyz>::bar<foo>", fsi.fReturnType);
   EXPECT_EQ("cde::fgh<ijk>", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("bar", fsi.fFunctionTemplateArguments[0]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("ret fgh<(i > 2 ? 3 : 4)>()", fsi);
   EXPECT_EQ("ret", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("fgh", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("(i > 2 ? 3 : 4)", fsi.fFunctionTemplateArguments[0]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("ret foo<a, b, c<d>, e = f<g>, h, ...>()", fsi);
   EXPECT_EQ("ret", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_EQ(6u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("a", fsi.fFunctionTemplateArguments[0]);
   EXPECT_EQ("b", fsi.fFunctionTemplateArguments[1]);
   EXPECT_EQ("c<d>", fsi.fFunctionTemplateArguments[2]);
   EXPECT_EQ("e = f<g>", fsi.fFunctionTemplateArguments[3]);
   EXPECT_EQ("h", fsi.fFunctionTemplateArguments[4]);
   EXPECT_EQ("...", fsi.fFunctionTemplateArguments[5]);
   EXPECT_TRUE(fsi.fFunctionParameters.empty());


   TClassEdit::SplitFunction("bar< foo(x::y), long long>                 "
                             "cde::fgh<ijk<x(y)>::bar>::foo<bar(a<b(c)>)>"
                             "(a::b<c::d<e::f(g::h)>*>::i j = k(l,m<n,p::o<q(r)>>))", fsi);
   EXPECT_EQ("bar< foo(x::y), long long>", fsi.fReturnType);
   EXPECT_EQ("cde::fgh<ijk<x(y)>::bar>", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_EQ(1u, fsi.fFunctionTemplateArguments.size());
   EXPECT_EQ("bar(a<b(c)>)", fsi.fFunctionTemplateArguments[0]);
   EXPECT_EQ(1u, fsi.fFunctionParameters.size());
   EXPECT_EQ("a::b<c::d<e::f(g::h)>*>::i j = k(l,m<n,p::o<q(r)>>)", fsi.fFunctionParameters[0]);

   TClassEdit::SplitFunction("someNamespace::someClass< std::function<void (sometype&)> >::FunctionName(std::function<void (someothertype&)>)", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("someNamespace::someClass< std::function<void (sometype&)> >", fsi.fScopeName);
   EXPECT_EQ("FunctionName", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_EQ(1u, fsi.fFunctionParameters.size());
   EXPECT_EQ("std::function<void (someothertype&)>", fsi.fFunctionParameters[0]);   
}

TEST(TClassEdit, SplitFuncErrors)
{
   TClassEdit::FunctionSplitInfo fsi;

   TClassEdit::SplitFunction("foo:", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo:", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction(":foo", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ(":foo", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction(":::", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ(":", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("a:::b", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("a", fsi.fScopeName);
   EXPECT_EQ(":b", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("a:b:c:d", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("a:b:c:d", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo(", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("(foo", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo)", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo)", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction(")foo", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ(")foo", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo<", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("<foo", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction("foo>", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ("foo>", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());

   TClassEdit::SplitFunction(">foo", fsi);
   EXPECT_EQ("", fsi.fReturnType);
   EXPECT_EQ("", fsi.fScopeName);
   EXPECT_EQ(">foo", fsi.fFunctionName);
   EXPECT_TRUE(fsi.fFunctionTemplateArguments.empty());
   EXPECT_TRUE(fsi.fFunctionParameters.empty());
}
