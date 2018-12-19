# An Implementation of GPU Accelerated 2D OTSU and 2D Entropy-based Thresholding

## Introduction
The source code is implemented based on the Compute Shader, a shader stage in OpenGL for general-purpose computing on graphics processing units (GPGPU).
  
The 2D thresholding consists of 7 steps: 
- step 1 is smoothing,
- step 2 is 2D histogram generation,
- step 3 is auxiliary data initialization,
- step 4 is summed area table generation,
- step 5 is criteria calculation,
- step 6 is optimal threshold searching,
- step 7 is binarization.

## Examples
```cpp
cs::CThresholding2D T2D;
void allstep_gpu_rt(int is2DOTSU)
{
	bool keys[1000] = { false };
	freopen("960_pure-GPU.txt", "a+", stdout);
	printf("\n");
	printf("pure GPU running time\n(\n 1: sp\n 2: hist2D\n 3: sat init\n 4: sat\n 5: compute criteria\n 6: reduce\n 7: binarize\n)\n");

	int graylevelst[3] = { 256, 512, 1024 };
	string filelist[] = { "256-01.bmp", "512-01.bmp", "1024-01.bmp", "1920-01.bmp" };
	
	for (int gi = 0; gi < 3;gi++)	{
		for (int fi = 0; fi < 4; fi++)	{
			for (int radius = 1; radius < 6; radius += 2)	{
				
				int gl = graylevelst[gi];
				T2D.setup1(filelist[fi].c_str(), keys, gl);

				int nf = 100;

				GLuint query[8];
				GLuint64  timeer[8];
				int done = 0;
				
				glGenQueries(8, query);
				glQueryCounter(query[0], GL_TIMESTAMP);
				for (int i = 1; i < nf; i++){
					T2D.smooth_sf(radius);
					//T2D.smooth(1);
				}
				glQueryCounter(query[1], GL_TIMESTAMP);
				for (int i = 1; i < nf; i++){
					T2D.genhist_v2();
				}
				glQueryCounter(query[2], GL_TIMESTAMP);
				for (int i = 1; i < nf; i++){
					T2D.init_sat(is2DOTSU);
				}
				glQueryCounter(query[3], GL_TIMESTAMP);
				////			//T2D.sat();
				for (int i = 1; i < nf; i++){
					T2D.sat_block();
				}
				glQueryCounter(query[4], GL_TIMESTAMP);
				for (int i = 1; i < nf; i++){
					T2D.cal_criteria(is2DOTSU);
				}
				glQueryCounter(query[5], GL_TIMESTAMP);
				////					//T2D.select_most_stride(2);
				for (int i = 1; i < nf; i++){
					T2D.select_most_shared(7);
				}
				glQueryCounter(query[6], GL_TIMESTAMP);
				for (int i = 1; i < nf; i++){
					T2D.binary();
				}
				glQueryCounter(query[7], GL_TIMESTAMP);

				while (!done) {
					glGetQueryObjectiv(query[7], GL_QUERY_RESULT_AVAILABLE, &done);
				}
				glGetQueryObjectui64v(query[0], GL_QUERY_RESULT, &(timeer[0]));
				for (int s = 1; s < 8; s++)	{
					glGetQueryObjectui64v(query[s], GL_QUERY_RESULT, &(timeer[s]));
					printf("gLevel=%4d file=%12s r=%d step%d: %7.3f ms\n", gl, filelist[fi].c_str(), radius, s, (timeer[s] - timeer[s - 1]) / 1000000.0 / nf);
				}
				
			}
		}
	}
}
```

## Dependencies
[GLEW 2.0.0 or above](http://glew.sourceforge.net/)
  
[GLFW 3.0.1 or above](https://www.glfw.org/)
