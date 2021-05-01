# How linking works: https://stackoverflow.com/questions/62871714/node-gyp-not-linking-libraries-correctly-on-macos
# variables: https://github.com/nodejs/node-gyp/issues/1223
# https://github.com/nodejs/node-gyp/issues/892
# https://medium.com/devgorilla/node-gyp-support-in-alpine-linux-9374c6191140
{
  'targets': [
    {
      'target_name': 'king-reward-solver-native',
      'sources': [ 'src/king_reward_solver.cc', 'src/captcha.cc' ],
      'include_dirs': [
        "src/include",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      
      'conditions': [
        ['OS=="win"', {
          'libraries': ["-lopencv_core451", "-lopencv_imgcodecs451", "-lopencv_imgproc451", "-lopencv_ml451"],
          'link_settings': {
            'library_dirs': [
              "opencv-prebuilt/win/lib"
            ]
          },
        }],
        
        ['OS=="linux"', {
          'libraries': [
            "-lopencv_core", "-lopencv_imgcodecs", "-lopencv_imgproc", "-lopencv_ml"
          ]
        }],
        
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '10.7'
          },
          'libraries': [
            "-L<(module_root_dir)/opencv-prebuilt/mac/lib/",
            "-lopencv_core", "-lopencv_imgcodecs", "-lopencv_imgproc", "-lopencv_ml",
          ]
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
