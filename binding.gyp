{
  'targets': [
    {
      'target_name': 'king-reward-solver-native',
      'sources': [ 'src/king_reward_solver.cc', 'src/captcha.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'libraries': ["-lopencv_core451", "-lopencv_imgcodecs451", "-lopencv_imgproc451", "-lopencv_ml451"],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      
      'conditions': [
        ['OS=="win"', {
          'include_dirs': ["opencv-prebuilt/win/include"],
          'link_settings': {
            'library_dirs': [
              "opencv-prebuilt/win/lib"
            ]
          },
        }],
        
        ['OS=="linux"', {
          'include_dirs': ["opencv-prebuilt/linux/include"],
          'link_settings': {
            'library_dirs': [
              "opencv-prebuilt/linux/lib"
            ]
          },
        }],
        
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '10.7'
          },
        }]
      ]
    },
    {
      "target_name": "copy_output",
      "type": "none",
      "dependencies" : [ "king-reward-solver-native" ],
      "copies": [
        {
         'files': ['build/Release/king-reward-solver-native.node'],
         'destination': 'lib/native/<(OS)',
        }
      ],
    }
  ]
}
