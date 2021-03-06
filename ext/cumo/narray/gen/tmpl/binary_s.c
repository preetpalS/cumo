<% unless type_name == 'robject' %>
void <%="cumo_#{c_iter}_stride_kernel_launch"%>(char *p1, char *p2, char *p3, ssize_t s1, ssize_t s2, ssize_t s3, uint64_t n);
<% end %>

static void
<%=c_iter%>(cumo_na_loop_t *const lp)
{
    size_t  i;
    char    *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    CUMO_INIT_COUNTER(lp, i);
    CUMO_INIT_PTR(lp, 0, p1, s1);
    CUMO_INIT_PTR(lp, 1, p2, s2);
    CUMO_INIT_PTR(lp, 2, p3, s3);
    <% if type_name == 'robject' %>
    {
        dtype x, y;
        CUMO_SHOW_SYNCHRONIZE_FIXME_WARNING_ONCE("<%=name%>", "<%=type_name%>");
        for (; i--;) {
            CUMO_GET_DATA_STRIDE(p1,s1,dtype,x);
            CUMO_GET_DATA_STRIDE(p2,s2,dtype,y);
            x = m_<%=name%>(x,y);
            CUMO_SET_DATA_STRIDE(p3,s3,dtype,x);
        }
    }
    <% else %>
    <%="cumo_#{c_iter}_stride_kernel_launch"%>(p1,p2,p3,s1,s2,s3,i);
    <% end %>
}

/*
  Calculate <%=name%>(a1,a2).
  @overload <%=name%>(a1,a2)
  @param [Cumo::NArray,Numeric] a1  first value
  @param [Cumo::NArray,Numeric] a2  second value
  @return [Cumo::<%=class_name%>] <%=name%>(a1,a2).
*/
static VALUE
<%=c_func(2)%>(VALUE mod, VALUE a1, VALUE a2)
{
    cumo_ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    cumo_ndfunc_arg_out_t aout[1] = {{cT,0}};
    cumo_ndfunc_t ndf = { <%=c_iter%>, CUMO_STRIDE_LOOP, 2, 1, ain, aout };
    return cumo_na_ndloop(&ndf, 2, a1, a2);
}
