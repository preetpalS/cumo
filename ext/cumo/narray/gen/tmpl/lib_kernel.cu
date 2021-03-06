#include "cumo/narray_kernel.h"
<% unless type_name == 'robject' %>
#include "cumo/indexer.h"
#include "cumo/reduce_kernel.h"
<% end %>
#include <<%="cumo/types/#{type_name}_kernel.h"%>>

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

<% children.each do |c|%>
<%= "#{c.result}\n\n" %>
<% end %>

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif
