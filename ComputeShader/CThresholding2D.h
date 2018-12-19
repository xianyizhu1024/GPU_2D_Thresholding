#pragma  once
#include "globject/shader.h"
#include "globject/g_pfm.h"
namespace cs
{
	class CThresholding2D
	{
	public:
		

		Shader * m_psh_hist1d; //compute shader
		Shader * m_psh_smooth; 
		Shader *m_psh_binary; 
		Shader *m_psh_initsat; // ��ʼ�� sat
		Shader *m_psh_sat;  // ����sat
		Shader *m_psh_criteria;  // ���� ���б�׼
		Shader *m_psh_most;  // ������most
			Shader * m_psh_smooth_sf; //�����˲�
			Shader * m_psh_most_stride;  // ������most stride
			Shader * m_psh_histsh; 
			Shader * m_psh_satblock01;
			Shader * m_psh_satblock02;
			Shader * m_psh_satblock03;
			Shader * m_psh_most_shared01;
			Shader * m_psh_most_shared02;

		GLuint m_tex_pic;   // picture������
		GLuint m_tex_smooth_tmp;
		GLuint m_tex_smooth_intensity_ave; // ǿ�� �� ��ֵ ������
		GLuint m_tex_binary;   // ��ֵ���������
		GLuint m_image_pic; // pic ��ͼ��Ԫ   Ĭ�� 0
		GLuint m_atom_cnt_buf_bins; //ֱ��ͼ��ԭ�Ӽ���buf
		GPfm m_pic; // ���� pic
		std::string m_path_pic;
		int m_width_pic,m_height_pic;

		GLuint m_tex_bins;         // 1άֱ��ͼ
		GLuint m_image_bins;		// Ĭ�� �� 1
		GLuint m_tex_bins2d;		//2άֱ��ͼ
		GLuint m_image_bins2d;		// default �� 3

		GLuint m_tex_SATa;		//  
		GLuint m_image_SATa;	// default �� 0
		GLuint m_tex_SATb;      //
		GLuint m_image_SATb;	// default �� 2  
		GLuint m_tex_SATc;      //
		GLuint m_image_SATc;	// default �� 1  

		GLuint m_tex_criteria;		//  
		GLuint m_image_criteria;	// default �� 0

		GLuint m_tex_most;		//  
		GLuint m_image_most;	// default �� 1

		GLuint m_tex_most_tmp;		//  
		GLuint m_image_most_tmp;	// default �� 2

		int m_grayscale;  // �Ҷȼ���  Ĭ����256
		int m_nearbyorder; // �˲�����order Ĭ���� 1  �� 3x3
		const int WRK_grp_size_x =16, WRK_grp_size_y=16, WRK_grp_size_z=1;  // x=16 y=16 z=1 
		int  m_wrk_grp_x, m_wrk_grp_y, m_wrk_grp_z;
		
		bool *m_pkey; //ָ�򰴼�
		GPf4 m_tmp;
		
		void set_gpfm_pic(std::string path);
		void set_wrkgrp();
		void set_shader_hist1d(std::string path) { m_psh_hist1d = new Shader(path,true); }
		void set_shader_smooth(std::string path) { m_psh_smooth = new Shader(path, true); }
		void set_shader_binary(std::string path) { m_psh_binary = new Shader(path,true); }
		void set_shader_initsat(std::string path) { m_psh_initsat = new Shader(path, true); }
		void set_shader_sat(std::string path) { m_psh_sat = new Shader(path, true); }
		void set_shader_criteria(std::string path) { m_psh_criteria = new Shader(path, true); }
		void set_shader_most(std::string path){ m_psh_most = new Shader(path,true); }
			void set_shader_smooth_sf(std::string path) { m_psh_smooth_sf = new Shader(path, true); }
			void set_shader_most_stride(std::string path){ m_psh_most_stride = new Shader(path, true); }
			void set_shader_hist2d(std::string path) { m_psh_histsh = new Shader(path, true); }
			void set_shader_sat_block(std::string path01, std::string path02, std::string path03) { 
				m_psh_satblock01 = new Shader(path01, true);
				m_psh_satblock02 = new Shader(path02, true);
				m_psh_satblock03 = new Shader(path03, true);
			}
			void set_shader_mostsh(std::string path) { m_psh_most_shared01 = new Shader(path, true); }


		void set_image_pic();
		void set_image_bins();
		void set_image_bins2d();
		void set_image_sat_criteria();
		void setup1(std::string pic_path, bool* pkey,int grayscale=256, int nearbyorder=1);
		void genhist(bool is1d);   // 2άֱ��ͼ
		void init_sat(int isOTSU=1);
		void sat();
		void cal_criteria(int isOTSU = 1);
		void select_most();
		void smooth(int r=1); // box filter
		void binary(GLuint texid_cst=-10);  //  texid of criterion s t
		void cal_criteriaCPU(bool isOTSU=true); // it begins with 'glGetTexImage' to acquire Hist2D from gpu, and ends with 'glTexSubImage2D' to transfer (s,t) to gpu
			void smooth_sf(int order = 1);// separable filter
			void select_most_stride(int stride=2);
			void genhist_v2();
			void sat_block();
			void select_most_shared(int stride=7);

		GLuint *g_pbins2d; // acquire Hist2D from gpu
	};


}