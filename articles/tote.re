= YAML上でLispが書けちゃうツールを作ってみた話

== はじめに
みなさんYAML、書いてますか。

AnsibleのPlaybookやDocker Compose、そして昨今は特に Kubernetes Manifest の記述で、YAMLを使う機会はグッと増えていると思います。

特に業務利用の場面では、「Kubernetesマニフェストを複数環境向けに少しずつ変えたバリエーションをいくつも用意したい」といったケースがよくあります。
そのために Helm や Kustomize といったツールも発達してきました。

が、今回はそれら既存ツールとはちょっと違うアプローチでこの「YAMLのバリエーション生成」を解決するツールを作ってみたので、本稿では「それが何なのか」「何が嬉しいのか」「既存のツールと何が違うのか」などを紹介していきます。

== どんなツール？

一言で言うと「YAML上でLisp的なコードが書け、評価できる」ツールです。これを「yisp」と命名しました。

yispについての説明をする前に、簡単にLispについておさらいしておこうと思います。

=== Lispとは
Lispは1960年くらいに開発されたプログラミング言語で、昨今だと「Lispエイリアン」などのミーム文脈で聞いた事があるという方は多いとは思いつつ、実際に書かれたことがある方は多くはないと思います。

最大の特徴は、「リストが主要なデータ構造」であり、コードとデータの構文が同じであることです。
たとえば Lisp で足し算をするとこう書きます：

//list[lisp_example][lispのサンプルコード][yaml]{
(+ 3 5)
//}

見どころは2つで、一つ目はコードがリスト表記になっていること。近代的に見覚えのある表記をすると、Lispのこのコードは要するに@<code>{["+", 3, 5]}というデータです。二つ目はコードがポーランド記法(前置記法)で書かれていることですね。この記法であれば、引数がいくつであってもどれも同じ構文で呼び出しを表現することができます。

この特徴が、YAMLのようなデータを表現する言語と相性が良いのではないかと考えました。

=== yisp
yispではYAMLのリスト構文を使ってプログラムを書けます。
ただし、YAML全体ではなく一部だけをyispとして評価したいため、評価したい部分の一番上に@<code>{!yisp}タグをつけます。
(タグはYAMLの中でも認知度がダントツに低い仕様の1つで値の「直前」に@<code>{!文字列}を記述するとその値にタグをつけることができます。タグはパーサーなど内部処理で利用することができます)

たとえばこういうYAMLを書き、

//list[example_source][sample.yaml][yaml]{
myaddition:
  !yisp
  - +
  - 3
  - 5
//}

yispで処理をすると
//list[example_build][yispによるビルド][cmd]{
yisp build sample.yaml
//}

次のようなYAMLが出力されます

//list[example_result][helmテンプレートの例][yaml]{
myaddition: 8
//}

ここから以下の2つのことが確認できますね。

  * yisp範囲外の構造はそのまま出力されていること
  * yispでの式が評価されて埋め込まれていること

== どうしてこんなことを？

「うわ！きしょ！」って思われた方もいると思います。わかります。

実のところこれは、Helm や Kustomize といった既存ツールを使っていて感じた不満が出発点です。それぞれについて少し詳しく見てみましょう。

=== helm
helmはいわゆるテンプレートエンジンで、次のような記述を取ります

//list[helm_example][helmテンプレートの例][yaml]{
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: {{ template "sample.fullname" . }}
  namespace: {{ .Release.Namespace }}
  labels:
{{ include "sample.labels" . | indent 4 }}
    app.kubernetes.io/component: agent
    {{- if .Values.agents.additionalLabels }}
{{ toYaml .Values.agents.additionalLabels | indent 4 }}
    {{- end }}
{{ include "provide
//}

これはgoのtext/templateパッケージをつかって実装されています。

ルールが簡潔でサクッと書ける一方、凝ったものを書こうと思うとどんどん複雑になっていきます。
特に、if-endの対応がぱっと見でわからないのがつらいですね。

つまりこれはYAMLをYAMLとして解釈するのではなく、テキストデータとして解釈する戦略をとっているわけですが、ずっとこれ一筋でやっていくのは困難でしょう。

=== kustomize
一方、Kustomize は YAML を YAML として扱いながら差分パッチを当てる思想のツールです。
特に「ベースに対して変更を加える」という運用に適しており、コンセプトは非常にモダンです。

ただし、記述されたリソースがどの順番で評価されるかが直感的ではありません。
時折「なんでこれがこうなるの？」というパズルを解く羽目になります。

また、「パラメーターから大規模に元となるマニフェスト群を生成する」ことはhelmと比べて得意ではありません。

=== yispは？

yispはhelmのようにYAMLを文字列として扱うのではなく、データとコードが同じというLispの思想を受け継ぎ、YAML上の要素としてコードが書けるようになりました。
また、kustomizeのように複雑な合成ルールを持たず、「関数の適用」という単純明快な評価モデルだけで動作するので、動作の予測が容易です。

== より実用的な例

=== テンプレート

もうちょっと実用的な例として、kubernetesマニフェストのようなものを出力させてみましょう。
たとえばテンプレートとして使うことができて、このようなYAMLをあらかじめ作っておきます

//list[template_template][template.yaml][yaml]{
!yisp &mkpod
- lambda
- [name, image]
- !quote
  apiVersion: v1
  kind: Pod
  metadata:
    name: *name
  spec:
    containers:
      - name: *name
        image: *image
//}

いきなり情報量が多いですが、ここでやっていることは
  * lambda演算子を使って、無名関数を作っている
  * YAMLのアンカー記法をつかって、この関数に@<code>{&mkpod}として名前をつける
  * この関数としてやりたいことは変数を埋め込んだデータを返すだけなので、一応@<code>{!quote}タグを使って不用意にyispとして評価されないようにする
  * YAMLのアンカー記法@<code>{*name}/@<code>{*image}をつかって、lambdaの引数をデータに埋め込む

ということをやっています。

これを別のファイルから読み出します。

//list[template_use][use.yaml][yaml]{
!yisp
- import
- ["template", "./template.yaml"]
---
!yisp
- *template.mkpod
- mypod1
- myimage1
//}

1つ目のドキュメントでは、import演算子でtemplate.yamlをtemplateという名前で読み込みます。
そして、二つ目のドキュメントで最初に定義したmypod関数を、@<code>{*template.mkpod}で使っています。
引数としてはmypod1とmyimage1を与えています。

これを評価すると、次のYAMLが得られます。

//list[template_result][出力結果][yaml]{
apiVersion: v1
kind: Pod
metadata:
  name: mypod1
spec:
  containers:
    - name: mypod1
      image: myimage1
//}

こうしてyispでもテンプレートのような利用ができましたね。

=== パッチを当てる

続いて、出力結果にパッチを当ててみましょう。metadata.nameのみを変更してみます。

//list[template_result][patch.yaml][yaml]{
!yisp
- merge
- - car
  - - include
    - use.yaml
- !quote
  metadata:
    name: patched-name
//}

use.yamlをincludeで呼び出して、評価したデータを取得します。yamlは1ファイルに複数ドキュメントが記載できるので、ここの戻り値は配列です。
そこで、配列の一番最初の要素を取り出すために@<code>{car}演算子を使っています。
最後に、@<code>{merge}演算子を使ってdeep mergeを行い、上書きします。

出力結果は次のようになります。

//list[template_result][出力結果][yaml]{
apiVersion: v1
kind: Pod
metadata:
  name: patched-name
spec:
  containers:
    - image: myimage1
      name: mypod1
//}

これで、metadata.nameだけを変更することができました。

=== configmapの生成

もう一つ実用的な例として、KubernetesのConfigMapを生成する例をみてみましょう。

//list[generate_config][generate-configmap.yaml][yaml]{
apiVersion: v1
kind: ConfigMap
metadata:
  name: my-config
data: !yisp
  - from-entries
  - - map
    - - lambda
      - [file]
      - !quote
        - *file.name
        - *file.body
    - - read-files
      - ./cm-files/*
//}

このYAMLでは以下のような処理を行っています。

* read-files 関数で cm-files/ 以下のファイルを全て読み込む
* map関数で各ファイルをnameとbodyのペアに変換
* from-entries関数でkey-valueのリストをYAMLのマッピングに変換し、dataに代入

この関数は、kustomizeのconfigmap-generatorと同じようなデザインになったいます。このように、yispの表現力を活用することでhelmの自由な記述と、kustomizeのようなデータの変換の両方をこなすことができます。

== yispでもっと遊んでみる

yispはただのテンプレートエンジンではなくLispとして(必要以上に)実装されているので、
たとえばZコンビネータ(不動点コンビネータの1つ)をつかった無名再帰のプログラムなんかが正常に動作します。

//list[z][Zコンビネータを使った無名再帰で階乗計算をする例][yaml]{
!yisp
&Z
- lambda
- [f]
- - - lambda
    - [x]
    - - *f
      - - lambda
        - [v]
        - - - *x
            - *x
          - *v
  - - lambda
    - [x]
    - - *f
      - - lambda
        - [v]
        - - - *x
            - *x
          - *v
---
!yisp
- - *Z
  - - lambda
    - [self]
    - - lambda
      - [n]
      - - if
        - [<=, *n, 1]
        - 1
        - - "*"
          - *n
          - - *self
            - - "-"
              - *n
              - 1
- 5
# => 120
//}

これは1つ目のドキュメントでZコンビネータを高階関数として定義して、2つ目のドキュメントで階乗のプログラムをZコンビネータに渡して評価しています。結果は120です。

実用的な例では全くなく、例えばkubernetesマニフェストを読むつもりでこれが出てきたら流石にキレると思いますが、エキシビションとしてはかなり面白いと思います。

== おわりに

yispは、「YAMLをあくまでYAMLとして保ちながら、柔軟なロジックを埋め込める」新しい選択肢です。
Lispライクな文法を採用することで、YAML内に強力な表現力を注入することを目指しました。

とはいえ、安全性や複雑性のトレードオフ・外部とのインターフェースなど課題も多く、絶賛開発中です。

OSSとして公開しているので、もし興味を持ってもらえたらぜひ触ってみてもらえると嬉しいです。

https://github.com/totegamma/yisp
