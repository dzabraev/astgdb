#include "SrcView.h"
#include "SourceFileAstVisitor.h"

namespace astgdb {

SourceViewer::extractSrcNodes(void) {
  const ASTContext *context = &ast->getASTContext();
  SourceFileAstVisitor visitor(context, nodes);
  visitor.TraverseDecl(context->getTranslationUnitDecl());
}

}//namespace astgdb
