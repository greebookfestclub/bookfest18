= 業務駆動開発のふりかえりとこれから

== はじめに

こんにちは、 @ganyariya @<fn>{ganyariya-twitter} と申します。
この章では、@<kw>{業務駆動開発}についての個人的なふりかえりと今後の展望をお話しします。

もしみなさんにおいても、業務駆動開発が自分にもあいそうという方がいらっしゃればぜひお試しください。

//caution{
    なお、@<kw>{業務駆動開発} は ganyariya の造語であることに注意してください。
//}

== 業務駆動開発とは: 業務でわからないことを個人開発でなんとなくわかるようにする駆動法

はじめに、@<kw>{業務駆動開発}とはどのような駆動法なのかについて説明します。

業務駆動開発とは、@<tt>{業務でわからなかったこと・これから使いそうなこと} を@<em>{個人開発でなんとなくわかる段階まで持っていく}駆動法です。@<em>{なんとなくわかる段階まで持っていったらあとは業務内で学び応用しつつ伸ばしていき}ます。

たとえば、お仕事をしていると以下のような状況に出くわすことがあります。

 * プロダクトで使っているツールについて、使い方は知っているもののそのツール自体の構築方法や仕組みがわかっていない
 * プロダクト内で発生している課題を解決できる技術が存在するが、その技術を触ったことがなく工数の検討がつかない

1 つのプロダクトを成り立たせるには数多くの技術が必要であり、当然わからないことはたくさんあります。
このわからないことを個人の範囲でキャッチアップし、それを業務で応用していくのが業務駆動開発です。

== どうして業務駆動開発を始めたのか

@<tt>{業務駆動開発} の用語説明を見たみなさんにとって、このような取り組みはエンジニアとして当たり前だろうとお思いになったはずです。自分も今ではそう思います。しかし、自分はこれを意識的に行えておらず、おそらく効率の悪い学習方法をとっていました。

この節では、業務駆動開発を始めたきっかけについて 2 つお話しします。

=== これまでの個人開発における効率が悪かった

1 つ目は、これまでの個人開発の効率が悪かったためです。

業務駆動開発を始める以前の ganyariya は、自分が興味ありそうな技術を手当たり次第に勉強していました。

 * インタプリタを Go で書いてみる @<fn>{ganyariya-go-interpreter}
 * DI コンテナを PHP で書いてみる @<fn>{ganyariya-php-di}
 * デザインパターンを Go で書いてみる @<fn>{ganyariya-go-design-patterns}
 * ドメイン駆動設計を Go で書いてみる @<fn>{ganyariya-go-ddd}

枯れた技術や設計（DDD / CleanArchitecture）の勉強を優先しており、これらは当然業務でも役立ちます。
しかし、業務で即時的に役立つものではないことも多かったです。
また、 ganyariya の興味は移ろいやすく、いろいろなものを並行した結果すべて中途半端になってしまうことも多くありました。

一方で、業務でわからないことに取り組むのであれば、必ず即時的に役立ち、かつ勉強方針がブレることがありません。
一つ一つわからないことを埋めていくだけでよいためです。

=== 業務でわからないことに挑戦したいが時間と機会がなかった

2 つ目は、業務中にわからないことに挑戦するような時間と機会がなかったためです。

本当であれば、業務中にわからないことを丁寧に調べて理解しながら応用する、が一番よいです。
しかし、プロダクトを運用しているとなかなかにタスクが多く、わからないことを丁寧に調べる、という時間はとれません。

また、タスクの割り振りをする際、チームメンバのアサイン状況が圧迫していると、工数を抑えるために該当の技術について経験がある方にタスクを振ることが多いと思います。これでは属人化が発生し、新たに学びたい人に機会を与えづらいです。

一方で、ある程度その技術に知識があれば工数も見積もりやすく、上長もタスクも振りやすくなります。

== 2024 における ganyariya の業務駆動開発例と効果

これらの理由から ganyariya は 2024 年から業務駆動開発を意識的に行いました。

この節では、ganyariya が行った業務駆動開発の例とその効果を紹介します。
詳細な内容については footnote のリンクを参照ください。

=== Kubernetes + ArgoCD

業務において、 Kuberntes + ArgoCD をつかってバックエンド API を提供しています @<fn>{wfs-k8s-argocd}。
ここで、 Kubernetes, ArgoCD の使い方はなんとなくわかっているものの、それぞれの構築方法は知らなかったため自分で組んでみることにしました @<fn>{ganyariya-house-k8s}。

ミニ PC を 1 台購入し、 k3s という Kubernetes Distribution をインストールして Kubernetes 環境を構築しました。
その後、 k8s 上に ArgoCD を導入し GitOps を実現しています。

ここで、 業務プロダクトの構成と同じく、 ArgoCD を ArgoCD 自体で管理しています @<fn>{ganyariya-argocd-by-argocd}。
また、 ArgoCD の Application を親 Application で管理する @<tt>{App of Apps Pattern} @<fn>{ganyariya-app-of-apps-pattern} を実現しています。

これだけだと k8s 環境に ArgoCD を載せただけで終わってしまうため、 Misskey を k8s 上で構築しました @<fn>{ganyariya-k8s-misskey}。

これらの取り組みによって、 Kubernetes と ArgoCD についてより理解を深められました。
とくに Kubernetes については、PVC の設定を変更して費用を削減できたり、 開発サーバがより安定するように VM の配置方法を変更できました。

個人で得られた経験をもとに業務でも活用できたと考えています。

=== Jenkins

業務において Jenkins をビルドパイプラインとして利用しています。
Jenkins の使い方はわかっているものの、どう構築するのかは k8s と同様にわからなかったため、こちらも自分で構築してみることにしました。

先ほどの ミニ PC k8s 環境に Jenkins をセットアップし、プロダクトと同様に Jenkins CasC で IaC 管理しました。
Jenkins を起動する際に自動でプラグインをインストールし、k8s 上の pod をワーカーノードとして利用できるようにしています。

また、Jenkins のワーカーノードをより安定化させたいという要件がプロダクトで発生しました。
ワーカーノードの一部として Google Cloud の VM を使っていたのですが、これら管理する社内ツールの設定値を変更する必要があり、業務駆動開発で先に学習しておくことにしました。

 * Jenkins に GCP VM をエージェントとして追加してみる @<fn>{ganyariya-jenkins-agent}
 * Ansible + Packer でイメージをビルドして GCP にデプロイする @<fn>{ganyariya-ansible-packer}

ここで学習した内容をもとに、プロダクトの GCP VM の設定値を変更しました。
その結果、より安定してワーカーノードが稼働するようになり、無事要件を達成できました。

=== o11y

OpenTelemetry で開発環境のトレースをとって Grafana で可視化できるようにしたい、というプロダクトの要件がありました。
これを実現するために、業務駆動開発で o11y ならびに OpenTelemetry を学習しました。

手始めに簡単な Go ウェブアプリに o11y ツールを導入することから始めました @<fn>{ganyariya-go-o11y}。
OpenTelemetry Collector を経由して Grafana Tempo にトレースを送り、それを Grafana で可視化できるようにする、という流れを経験できました。

くわえて、Grafana と Loki についてより詳しくなるために helm-stack を活用してミニ PC k8s をモニタリングできるようにしました @<fn>{ganyariya-house-k8s}。

これらの経験をもとに、プロダクト内で OpenTelemetry を導入し、開発環境のトレースを Grafana 上で確認できるようにしました。
とはいってもトレースを取るための計装コード生成は先輩の方が作ったコードを利用していますし、 Tempo に送信するための Grafana Alloy はインフラの方にご用意いただいています。
それでもこれまで知らなかった OpenTelemetry や o11y を学ぶことができ、それをプロダクトに応用できたのはとても楽しくよい経験でした。

== 業務駆動開発のメリットとデメリット比較

2024 年に業務駆動開発を行ったうえで、自分なりのメリットとデメリットをまとめておきます。

=== メリット

 * これまで知らなかったことを能動的に学べる
 * 新しい技術を使ったタスクに取り組める機会が増える（はず）
 * 破壊してもよいため気持ちが楽
 * 枯れた技術を学べる
 * お給料があがる（成果を出せばですが...）

やはりこれまで知らなかったことを能動的に学べるのが一番のメリットです。知らないから、他職種の範囲だから、と切り捨てるのではなく、積極的に挑戦でき新しいものを知ることができます。
また、個人の環境のため好き勝手に作業して壊してよいところも大きいです。業務で破壊してしまうとサービスや開発環境に影響を与えてしまうため...。

=== デメリット

 * 長期的に必要な勉強が後回しになる
 * アウトプットが業務のみになる

とくに、アウトプットが業務のみになるのが業務駆動開発の大きなデメリットかなと思いました。
個人の範囲で入口を学ぶため、応用する場所が業務になってしまいます。

とはいっても、これは業務駆動開発にかかわらず、ポートフォリオや OSS を個人で開発するのがモチベーションや時間の観点から難しいというのが原因でもあります。

== 課題に対する解決策としてアウトプット駆動も取り入れる

業務駆動開発によって、個人で学び業務で応用する、というよいサイクルを回せました。
しかし、個人では入口のみ学ぶため、個人エンジニアとしてのアウトプットが出せずポートフォリオが増えないという課題が残りました。

そのため、今後は業務駆動開発に並行して、アウトプット駆動開発も取り入れようと思います。
個人で学び業務で応用しつつ、それを個人開発でもアウトプットしてリリースする、というフローを目指そうと思います。
Vibe Coding も流行ってきており、どんどん個人開発の壁は低くなってきています。今がそのときですね...。

== まとめ

業務駆動開発は、業務でわからないことを個人開発でなんとなくわかるようにする駆動法です。
みなさんももし業務で使っているものでよくわかっていないんだよなというものがあれば、個人ではじめて業務で応用する、というフローをお試しいただけばと思います。

//footnote[ganyariya-twitter][https://twitter.com/ganyariya]
//footnote[ganyariya-go-interpreter][https://github.com/ganyariya/go_monkey]
//footnote[ganyariya-php-di][https://github.com/ganyariya/Hako]
//footnote[ganyariya-go-design-patterns][https://github.com/ganyariya/design_pattern]
//footnote[ganyariya-go-ddd][https://github.com/ganyariya/itddd_go]
//footnote[wfs-k8s-argocd][https://note.com/wfs_blog/n/n11d137738919]
//footnote[ganyariya-house-k8s][https://zenn.dev/ganariya/articles/202410-current-home-server-and-prospect]
//footnote[ganyariya-argocd-by-argocd][https://qiita.com/sourjp/items/4547bc81c12286007553]
//footnote[ganyariya-app-of-apps-pattern][https://dev.classmethod.jp/articles/argocd-app-of-apps-pattern/]
//footnote[ganyariya-k8s-misskey][https://zenn.dev/ganariya/articles/setup-misskey-by-kubernetes]
//footnote[ganyariya-jenkins-agent][https://zenn.dev/ganariya/articles/trial-jenkins-master-agent-connection]
//footnote[ganyariya-ansible-packer][https://github.com/ganyariya/playground/tree/main/gcp-packer]
//footnote[ganyariya-go-o11y][https://github.com/ganyariya/playground/tree/main/simple-go-observe]
