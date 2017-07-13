import { Component, Input, OnInit } from '@angular/core';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { SeerConnectionService } from 'app/services/seer-connection/seer-connection.service';
import { Observable } from 'rxjs/Observable';

@Component({
  selector: 'app-connection',
  templateUrl: './connection.component.html',
  styleUrls: ['./connection.component.scss']
})
export class ConnectionComponent implements OnInit {

  @Input() public connection: ConnectionMeta;

  protected dataPoints: any[] = [];
  private seerObservable: Observable<any>;

  constructor(private seerConnection: SeerConnectionService) { }

  public ngOnInit() {
    this.seerObservable = this.seerConnection.connect(`ws://${this.connection.address}:9000`);
    this.seerObservable.subscribe((message: any[]) => {
      // console.log('received message from server: ', message);
      this.dataPoints.push.apply(this.dataPoints, message);
    });
  }

}
