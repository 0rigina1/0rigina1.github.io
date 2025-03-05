---
title: DRF(Django REST Framework)在view中动态配置renderer
date: 2025-03-06 01:05:15
tags: 后端
---


## 1、自定义AESEncRenderer
自定义一个Renderer，作用是对view的response进行aes加密。
```python
from .encryptions.aes import AESCipher
from rest_framework.renderers import JSONRenderer
from backend.settings import AES_KEY


class AESEncJSONRenderer(JSONRenderer):

    def __init__(self):
        super().__init__()
        self.cipher = AESCipher(AES_KEY)

    def render(self, data, accepted_media_type=None, renderer_context=None) -> bytes:
        original_json = super().render(data, accepted_media_type, renderer_context)

        try:
            encrypted_data = self.cipher.encrypt(original_json)
            enc_data = {
                'c': 1,
                'd': encrypted_data,
            }
            return super().render(enc_data, accepted_media_type, renderer_context)
        except Exception as e:
            raise RuntimeError(f'响应加密失败: {e}')
```

## 2、设置全局默认renderer为AESEncRenderer
将AESEncJSONRenderer设置为全局默认响应渲染器
```python
REST_FRAMEWORK = {
    # ...
    'DEFAULT_RENDERER_CLASSES': [
        'path.your.renderer.AESEncJSONRenderer',
        # ...  
    ],
    # ...
}
```

## 3、在view中根据请求参数动态配置renderer
请求体中的render参数控制renderer
```python
from backend.common.response import success
from rest_framework.views import APIView
from rest_framework.renderers import JSONRenderer


class AuthByAccount(APIView):
    
    def post(self, request):
        renderers = {
            'json': JSONRenderer(),
            # 'foo': 'bar',
        }
        renderer_type = self.request.data.get('render')
        request.accepted_renderer = renderers.get(renderer_type, super().get_renderers())
        
        return success(data={'userId': 1})
```
render=json时，返回明文json响应

![明文json响应](image1.png)

render未设置时，返回AES加密后的json响应

![aes加密json响应](image2.png)