#include <soul/include/parser/parse_api.h>

#include <soul/include/common/lex_context.h>
#include <soul/include/parser/lexer.h>
#include <soul/include/parser/grammars.h>
#include <soul/include/parser/diags.h>
#include <soul/include/common/diag_chat.h>

#include <eflib/include/diagnostics/assert.h>

#include <eflib/include/platform/boost_begin.h>
#include <boost/format.hpp>
#include <eflib/include/platform/boost_end.h>

#include <iostream>

namespace soul{
	namespace common{
		class diag_chat;
	}
}
using soul::common::lex_context;
using soul::common::code_source;
using soul::common::diag_chat;
using boost::shared_ptr;
using std::cout;
using std::endl;

bool soul::parser::parse( 
	shared_ptr<attribute>& pt_root,
	const std::string& code,
	shared_ptr<lex_context> ctxt,
	lexer& l, grammars& g, diag_chat* diags
	)
{
	soul::parser::token_seq toks;
	
	bool tok_result = l.tokenize_with_end(code, ctxt, toks);
	if(!tok_result)
	{
		diags->report( soul::parser::unrecognized_token )
			->file( ctxt->file_name() )->span( soul::common::code_span(ctxt->line(), ctxt->column(), 1) )
			->p("<unknown>");
		return false;
	}

	token_iterator it = toks.begin();
	return g.prog.parse( it, toks.end()-1, pt_root, diags ).is_succeed();
}

bool soul::parser::parse( 
	shared_ptr<attribute>& pt_root,
	code_source* src,
	shared_ptr<lex_context > ctxt,
	lexer& l, grammars& g, diag_chat* diags
	)
{
	soul::parser::token_seq toks;

	l.begin_incremental();
	try
	{
		while( !src->eof() ){
			std::string next_token = src->next();
			bool tok_result = l.incremental_tokenize( next_token, ctxt, toks );
			if( !tok_result ){
				diags->report( soul::parser::unrecognized_token )
					->file( ctxt->file_name() )->span( soul::common::code_span(ctxt->line(), ctxt->column(), 1) )
					->p(next_token);
				return false;
			}
		}
	}
	catch(...)
	{
		l.end_incremental( ctxt, toks );
		return false;
	}
	
	l.end_incremental( ctxt, toks );
	token_iterator it = toks.begin();
	return !src->failed() && g.prog.parse( it, toks.end()-1, pt_root, diags ).is_succeed();
}
