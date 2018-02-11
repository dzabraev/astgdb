#ifndef __source_file_ast_visitor_h__
#define __source_file_ast_visitor_h__

#include <vector>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"

#include "SrcNode.h"

using namespace clang;

namespace astgdb {

/// https://clang.llvm.org/docs/RAVFrontendAction.html
class SourceFileAstVisitor
  : public RecursiveASTVisitor<SourceFileAstVisitor> {
  public:
    explicit SourceFileAstVisitor(const ASTContext *Context,
        std::vector<SrcNode> *nodes)
     : Context(Context),
       SourceMgr(&Context->getSourceManager()),
       nodes(nodes) {}
     
    bool VisitCompoundStmt(CompoundStmt *S);
  
  private:
    const ASTContext *Context;
    const SourceManager *SourceMgr;
    std::vector<SrcNode> *nodes;
};

}//namespace astgdb

#endif
