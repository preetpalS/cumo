#undef int_t
#define int_t unsigned long long int

__global__ void <%="cumo_#{c_iter}_index_kernel"%>(size_t p1, char* p2, CUMO_BIT_DIGIT *a1, size_t *idx1, uint64_t n)
{
    for (uint64_t i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        CUMO_BIT_DIGIT x=0;
        CUMO_LOAD_BIT(a1, p1 + idx1[i], x);
        if (m_<%=name%>(x)) {
            atomicAdd((int_t*)p2, (int_t)1);
        }
    }
}

__global__ void <%="cumo_#{c_iter}_stride_kernel"%>(size_t p1, char* p2, CUMO_BIT_DIGIT *a1, ssize_t s1, uint64_t n)
{
    for (uint64_t i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        CUMO_BIT_DIGIT x=0;
        CUMO_LOAD_BIT(a1, p1 + i * s1, x);
        if (m_<%=name%>(x)) {
            atomicAdd((int_t*)p2, (int_t)1);
        }
    }
}

__global__ void <%="cumo_#{c_iter}_index_stride_kernel"%>(size_t p1, char* p2, CUMO_BIT_DIGIT *a1, size_t *idx1, ssize_t s2, uint64_t n)
{
    for (uint64_t i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        CUMO_BIT_DIGIT x=0;
        CUMO_LOAD_BIT(a1, p1 + idx1[i], x);
        if (m_<%=name%>(x)) {
            atomicAdd((int_t*)(p2 + i * s2), (int_t)1);
        }
    }
}

__global__ void <%="cumo_#{c_iter}_stride_stride_kernel"%>(size_t p1, char* p2, CUMO_BIT_DIGIT *a1, ssize_t s1, ssize_t s2, uint64_t n)
{
    for (uint64_t i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        CUMO_BIT_DIGIT x=0;
        CUMO_LOAD_BIT(a1, p1 + i * s1, x);
        if (m_<%=name%>(x)) {
            atomicAdd((int_t*)(p2 + i * s2), (int_t)1);
        }
    }
}

void <%="cumo_#{c_iter}_index_kernel_launch"%>(size_t p1, char *p2, CUMO_BIT_DIGIT *a1, size_t *idx1, uint64_t n)
{
    size_t grid_dim = cumo_get_grid_dim(n);
    size_t block_dim = cumo_get_block_dim(n);
    <%="cumo_#{c_iter}_index_kernel"%><<<grid_dim, block_dim>>>(p1,p2,a1,idx1,n);
}

void <%="cumo_#{c_iter}_stride_kernel_launch"%>(size_t p1, char *p2, CUMO_BIT_DIGIT *a1, ssize_t s1, uint64_t n)
{
    size_t grid_dim = cumo_get_grid_dim(n);
    size_t block_dim = cumo_get_block_dim(n);
    <%="cumo_#{c_iter}_stride_kernel"%><<<grid_dim, block_dim>>>(p1,p2,a1,s1,n);
}

void <%="cumo_#{c_iter}_index_stride_kernel_launch"%>(size_t p1, char *p2, CUMO_BIT_DIGIT *a1, size_t *idx1, ssize_t s2, uint64_t n)
{
    size_t grid_dim = cumo_get_grid_dim(n);
    size_t block_dim = cumo_get_block_dim(n);
    <%="cumo_#{c_iter}_index_stride_kernel"%><<<grid_dim, block_dim>>>(p1,p2,a1,idx1,s2,n);
}

void <%="cumo_#{c_iter}_stride_stride_kernel_launch"%>(size_t p1, char *p2, CUMO_BIT_DIGIT *a1, ssize_t s1, ssize_t s2, uint64_t n)
{
    size_t grid_dim = cumo_get_grid_dim(n);
    size_t block_dim = cumo_get_block_dim(n);
    <%="cumo_#{c_iter}_stride_stride_kernel"%><<<grid_dim, block_dim>>>(p1,p2,a1,s1,s2,n);
}

#undef int_t
